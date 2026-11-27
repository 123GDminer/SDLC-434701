// Электронный путеводитель. Исходы чтения базы каталога.
package CatalogStates {
    item def 'Записи прочитаны';
    item def 'Записей нет';
    item def 'Ошибка чтения';

    state def <Lifecycle> 'Загрузка каталога' {
        entry; then loading;
        state <loading> 'Чтение';
        state <available> 'Списки доступны';
        state <empty> 'Нет записей';
        state <failed> 'Ошибка загрузки';
        transition first loading
            accept : 'Записи прочитаны' then available;
        transition first loading
            accept : 'Записей нет' then empty;
        transition first loading
            accept : 'Ошибка чтения' then failed;
    }
}
