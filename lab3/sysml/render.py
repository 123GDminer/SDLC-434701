# Электронный путеводитель. Проверка моделей и экспорт диаграмм SysML v2.
import argparse
import json
import os
from pathlib import Path
from queue import Empty
import re
import subprocess
from tempfile import TemporaryDirectory
from time import monotonic

from jupyter_client import BlockingKernelClient
from jupyter_client.connect import write_connection_file
from PIL import Image
from resvg_py import svg_to_bytes


def execute(client, code):
    message_id = client.execute(code, allow_stdin=False)
    deadline = monotonic() + 100
    outputs = []
    while monotonic() < deadline:
        message = client.get_iopub_msg(timeout=max(1, deadline - monotonic()))
        if message['parent_header'].get('msg_id') != message_id:
            continue
        content = message['content']
        kind = message['msg_type']
        if kind == 'status' and content['execution_state'] == 'idle':
            return outputs
        if kind == 'error' or kind == 'stream' and content['name'] == 'stderr':
            raise RuntimeError(str(content))
        if kind in ('stream', 'execute_result', 'display_data'):
            outputs.append(content.get('data', {'text/plain': content.get('text', '')}))
    raise TimeoutError('Операция в ядре SysML v2 не завершилась.')


def wait_for_kernel(client, process):
    deadline = monotonic() + 90
    while monotonic() < deadline and process.poll() is None:
        client.kernel_info()
        try:
            message = client.get_shell_msg(timeout=1)
            if message['msg_type'] == 'kernel_info_reply':
                return
        except Empty:
            continue
    raise RuntimeError(f'Ядро SysML v2 недоступно. Код завершения: {process.poll()}')


def export(client, project_dir, output):
    source = project_dir / 'models'
    for model in sorted(source.glob('*.sysml')):
        result = execute(client, model.read_text(encoding='utf-8'))
        diagnostics = '\n'.join(item.get('text/plain', '') for item in result)
        print(f'{model.name}: модель загружена', flush=True)
        if re.search(r'(?im)\b(error|warning|exception)s?\b', diagnostics):
            raise RuntimeError(f'{model.name}: {diagnostics}')

    output.mkdir(parents=True, exist_ok=True)
    for line in (project_dir / 'views.txt').read_text(encoding='utf-8').splitlines():
        name, view, element = line.split(maxsplit=2)
        result = execute(client, f'%viz --view={view} {element}')
        svg = next((item['image/svg+xml'] for item in result if 'image/svg+xml' in item), None)
        if not svg:
            raise RuntimeError(f'{name}: {result}')
        (output / f'{name}.svg').write_text(svg, encoding='utf-8', newline='\n')
        (output / f'{name}.png').write_bytes(svg_to_bytes(svg_string=svg, zoom=3))
        print(f'{name}: {view} {element}', flush=True)

    with Image.open(output / '03-usecases-reader.png') as reader, \
         Image.open(output / '03-usecases-author.png') as author:
        spacing = 48
        width = max(reader.width, author.width)
        height = reader.height + author.height + spacing
        diagram = Image.new('RGB', (width, height), 'white')
        diagram.paste(reader, ((width - reader.width) // 2, 0), reader)
        diagram.paste(author, ((width - author.width) // 2, reader.height + spacing), author)
        diagram.save(output / '03-usecases.png')


def main():
    project_dir = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__ or 'Экспорт диаграмм SysML v2')
    parser.add_argument('--kernel-home', required=True, type=Path)
    parser.add_argument('--dot', required=True, type=Path)
    parser.add_argument('--out', type=Path, default=project_dir / 'rendered')
    args = parser.parse_args()

    kernel = args.kernel_home.resolve(strict=True)
    specification = json.loads((kernel / 'kernel.json').read_text(encoding='utf-8'))
    env = dict(os.environ, **specification.get('env', {}))
    env['ISYSML_LIBRARY_PATH'] = (kernel / 'sysml.library').as_posix()
    env['ISYSML_GRAPHVIZ_PATH'] = args.dot.resolve(strict=True).as_posix()
    with TemporaryDirectory(prefix='guide-sysml-', ignore_cleanup_errors=True) as job:
        connection_file, _ = write_connection_file(str(Path(job) / 'connection.json'))
        argv = [arg.replace('@KERNEL_INSTALL_DIRECTORY@', kernel.as_posix())
                .replace('{connection_file}', connection_file)
                for arg in specification['argv']]
        java_home = os.environ.get('JAVA_HOME')
        if java_home and argv[0] == 'java':
            argv[0] = str(Path(java_home) / 'bin' / 'java')
        client = BlockingKernelClient(connection_file=connection_file)
        client.load_connection_file()
        with (Path(job) / 'kernel.log').open('w+', encoding='utf-8',
                                              errors='replace') as log:
            process = subprocess.Popen(argv, cwd=kernel, env=env, stdout=log,
                                       stderr=subprocess.STDOUT)
            try:
                client.start_channels()
                wait_for_kernel(client, process)
                export(client, project_dir, args.out)
            except Exception:
                log.seek(0)
                print(''.join(log.readlines()[-12:]), flush=True)
                raise
            finally:
                client.stop_channels()
                process.terminate()
                process.wait(timeout=10)


if __name__ == '__main__':
    main()
