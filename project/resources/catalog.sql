-- Электронный путеводитель. Схема SQLite и демонстрационные записи каталога.
PRAGMA foreign_keys = ON;
BEGIN;

CREATE TABLE countries (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE CHECK (length(trim(name)) > 0),
    description TEXT NOT NULL DEFAULT ''
);
CREATE TABLE attractions (
    id INTEGER PRIMARY KEY,
    country_id INTEGER NOT NULL REFERENCES countries(id),
    name TEXT NOT NULL CHECK (length(trim(name)) > 0),
    city TEXT NOT NULL DEFAULT '',
    description TEXT NOT NULL DEFAULT ''
);
CREATE TABLE routes (
    id INTEGER PRIMARY KEY,
    country_id INTEGER NOT NULL REFERENCES countries(id),
    name TEXT NOT NULL CHECK (length(trim(name)) > 0),
    description TEXT NOT NULL DEFAULT ''
);
CREATE TABLE route_stops (
    route_id INTEGER NOT NULL REFERENCES routes(id),
    position INTEGER NOT NULL CHECK (position > 0),
    attraction_id INTEGER NOT NULL REFERENCES attractions(id),
    PRIMARY KEY (route_id, position)
);

INSERT INTO countries VALUES
    (1, 'Беларусь', 'Памятные места Минска и исторические объекты других городов.'),
    (2, 'Италия', 'Архитектура и исторические достопримечательности Рима.'),
    (3, 'Япония', 'Достопримечательности Киото.');

INSERT INTO attractions VALUES
    (1, 1, 'Площадь Независимости', 'Минск', 'Одна из центральных площадей Минска. Рядом находятся Дом правительства и Красный костёл.'),
    (2, 1, 'Троицкое предместье', 'Минск', 'Исторический квартал на берегу Свислочи. Для прогулки подойдут набережная и улицы рядом с ней.'),
    (3, 1, 'Верхний город', 'Минск', 'Исторический район вокруг площади Свободы. Здесь находятся городская ратуша и соборы.'),
    (4, 1, 'Брестская крепость', 'Брест', 'Мемориальный комплекс на территории исторической крепости.'),
    (5, 1, 'Лидский замок', 'Лида', 'Каменный замок с внутренним двором в центре Лиды.'),
    (6, 2, 'Колизей', 'Рим', 'Античный амфитеатр рядом с Римским форумом.'),
    (7, 2, 'Пантеон', 'Рим', 'Здание античного происхождения с куполом и круглым отверстием в его центре.'),
    (8, 2, 'Фонтан Треви', 'Рим', 'Фонтан в стиле барокко на одноимённой площади.'),
    (9, 3, 'Фусими Инари', 'Киото', 'Синтоистское святилище с тропами, вдоль которых установлены ворота тории.'),
    (10, 3, 'Киёмидзу-дэра', 'Киото', '');

INSERT INTO routes VALUES
    (1, 1, 'Прогулка по центру Минска', 'Маршрут от площади Независимости через Верхний город до Троицкого предместья.'),
    (2, 2, 'Прогулка по центру Рима', 'Маршрут от Колизея через Пантеон к фонтану Треви.');

INSERT INTO route_stops VALUES
    (1, 1, 1), (1, 2, 3), (1, 3, 2),
    (2, 1, 6), (2, 2, 7), (2, 3, 8);

COMMIT;
