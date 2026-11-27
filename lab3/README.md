# Электронный путеводитель. Лабораторная работа № 3

[Актуальный отчёт Word](lab3.docx) · [PDF](lab3.pdf) · [Спецификация](SDLC-lab3_business-requirements/srs.md)

Версия 0.9. В спецификации 20 требований с ID и критериями приёмки. В приложении А отчёта приведены исходники девяти моделей. Диаграммы и макеты получены из SysML v2. Рисунок 6.1 показывает историю изменений Git.

## Рисунки

- [01-business-goals](sysml/01-business-goals.png)
- [02-context](sysml/02-context.png)
- [03-usecases](sysml/03-usecases.png)
- [04-catalog-states](sysml/04-catalog-states.png)
- [05-review-flow](sysml/05-review-flow.png)
- [06-navigation](sysml/06-navigation.png)
- [07-gui-catalog](sysml/07-gui-catalog.png)
- [08-gui-route](sysml/08-gui-route.png)
- [09-gui-publication](sysml/09-gui-publication.png)
- [10-gui-review-photos](sysml/10-gui-review-photos.png)
- [14-git-history](sysml/14-git-history.png)
- [catalog-mockup-report](sysml/catalog-mockup-report.png)
- [route-mockup-report](sysml/route-mockup-report.png)

- [Распределение требований по типам](sysml/16-requirements-count.png)
- [Граница фильтра времени](sysml/17-duration-boundary.png)
- [Связи сущностей каталога](sysml/18-domain.png)
- [Карточка достопримечательности](sysml/19-attraction.png)
- [Экран ошибки загрузки](sysml/20-broken-catalog.png)
- [Обзор по ссылке](sysml/21-shared-review.png)

## Текстовые модели

- [01-context](sysml/models/01-context.sysml)
- [02-usecases](sysml/models/02-usecases.sysml)
- [03-business-goals](sysml/models/03-business-goals.sysml)
- [04-catalog-states](sysml/models/04-catalog-states.sysml)
- [05-review-flow](sysml/models/05-review-flow.sysml)
- [06-gui](sysml/models/06-gui.sysml)
- [review-photos](sysml/models/review-photos.sysml)

- [07-categories](sysml/models/07-categories.sysml)
- [08-duration-filter](sysml/models/08-duration-filter.sysml)

## Экспорт

Используются Java 25, Graphviz и Jupyter-ядро SysML v2 версии 0.63.1. Распакуйте [релиз ядра](https://github.com/Systems-Modeling/SysML-v2-Pilot-Implementation/releases/tag/2026-09.1) и укажите каталог `sysml`, содержащий `kernel.json`.

```powershell
python -m pip install jupyter-client Pillow resvg-py
python lab3/sysml/render.py --kernel-home PATH_TO_KERNEL/sysml --dot PATH_TO_DOT
```

[Представления](sysml/views.txt) · [Проверка и экспорт](sysml/render.log) · [SVG](sysml/svg/). На рисунке 5.3 два представления вариантов использования расположены друг под другом.
