// Электронный путеводитель. Карта экранов и элементы интерфейса.
package GuideGui {
    part def Screen;
    part def Field;
    part def Readout;
    part def EntryList;
    part def Control;

    part <navigation> 'Карта экранов' {
        part <catalog> 'Каталог' : Screen;
        part <route> 'Прогулка' : Screen;
        part <stop> 'Достопримечательность' : Screen;
        part <editor> 'Редактор отзыва' : Screen;
        part <shared> 'Обзор по ссылке' : Screen;
        part <external> 'Внешняя ссылка';
        connection 'открыть прогулку' connect catalog to route;
        connection 'выбрать остановку' connect route to stop;
        connection 'создать отзыв' connect stop to editor;
        connection 'опубликовать' connect editor to shared;
        connection 'открыть обзор' connect external to shared;
    }

    part <catalogScreen> 'Каталог страны' : Screen {
        part <country> 'Выбор страны' : Field;
        part <limit> 'Лимит, минут' : Field;
        part <places> 'Достопримечательности' : EntryList {
            part 'Колизей' : Control;
            part 'Пантеон' : Control;
        }
        part <routes> 'Маршруты' : EntryList {
            part 'Прогулка по Риму, 90 мин' : Control;
        }
        connection 'обновить места' connect country to places;
        connection 'обновить прогулки' connect country to routes;
        connection 'фильтр' connect limit to routes;
    }

    part <routeScreen> 'Карточка маршрута' : Screen {
        part <title> 'Прогулка по Риму' : Readout;
        part <country> 'Италия' : Readout;
        part <duration> '90 минут' : Readout;
        part <stops> 'Остановки' : EntryList {
            part '1 Колизей' : Control;
            part '2 Пантеон' : Control;
            part '3 Фонтан Треви' : Control;
        }
        connection 'открывает список' connect title to stops;
    }

    part <placeScreen> 'Карточка места' : Screen {
        part <place> 'Пантеон' : Readout;
        part <description> 'Описание достопримечательности' : Readout;
        part <position> 'Остановка 2 из 3' : Readout;
        part <controls> 'Переходы' {
            part 'К Колизею' : Control;
            part 'К фонтану Треви' : Control;
            part 'К прогулке' : Control;
        }
        connection 'показать описание' connect place to description;
        connection 'доступные переходы' connect position to controls;
    }

    part <errorScreen> 'Недоступный каталог' : Screen {
        part <warning> 'Не удалось открыть каталог' : Readout;
        part <hidden> 'Неполные записи скрыты' : Readout;
        connection 'результат отказа' connect warning to hidden;
    }

    part <editorScreen> 'Редактор отзыва' : Screen {
        part <reviewedPlace> 'Достопримечательность' : Field;
        part <text> 'Текст отзыва' : Field;
        part <pictures> 'Фотографии' : EntryList {
            part 'Предпросмотр снимка 1' : Control;
            part 'Предпросмотр снимка 2' : Control;
        }
        part <attach> 'Добавить фотографии' : Control;
        part <publish> 'Опубликовать' : Control;
        connection 'добавление снимков' connect attach to pictures;
        connection 'сохранить текст' connect text to publish;
        connection 'сохранить вложения' connect pictures to publish;
    }

    part <sharedScreen> 'Обзор по ссылке' : Screen {
        part <link> 'Адрес публикации' : Field;
        part <reviewedPlace> 'Связанное место' : Readout;
        part <text> 'Текст отзыва' : Readout;
        part <pictures> 'Фотографии прогулки' : EntryList {
            part 'Снимок 1' : Readout;
            part 'Снимок 2' : Readout;
        }
        connection 'поиск объекта' connect link to reviewedPlace;
        connection 'показ текста' connect reviewedPlace to text;
        connection 'показ вложений' connect reviewedPlace to pictures;
    }
}
