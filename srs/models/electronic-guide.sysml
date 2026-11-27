// Электронный путеводитель. Участники и границы системы.
package GuideContext {
    part def Guide;
    part def Visitor;
    part def Reader :> Visitor;
    part def Author :> Visitor;
    part def Repository;
    part def CommunicationChannel;

    part <context> 'Контекст путеводителя' {
        part <app> 'Путеводитель' : Guide;
        part <reader> 'Читатель' : Reader;
        part <author> 'Автор' : Author;
        part <catalog> 'База каталога' : Repository;
        part <reviews> 'Сервис обзоров' : Repository;
        part <channel> 'Канал обмена' : CommunicationChannel;
        connection 'выбор и просмотр' connect reader to app;
        connection 'создание материала' connect author to app;
        connection 'чтение записей' connect app to catalog;
        connection 'публикация' connect app to reviews;
        connection 'передача ссылки' connect author to channel;
        connection 'получение ссылки' connect channel to reader;
    }
}

// Электронный путеводитель. Варианты использования и их связи.
package GuideUseCases {
    package <ReaderCases> 'Сценарии читателя' {
        private import GuideContext::*;

        use case 'Просмотреть каталог' {
            subject guide : Guide;
            actor 'Читатель' : Reader;
        }
        use case 'Отобрать по времени' {
            subject guide : Guide;
            actor 'Читатель' : Reader;
        }
        use case 'Открыть маршрут' {
            subject guide : Guide;
            actor 'Читатель' : Reader;
            include use case 'Показать остановки' {
                subject guide;
            }
        }
        use case 'Открыть обзор' {
            subject guide : Guide;
            actor 'Читатель' : Reader;
        }
    }

    package <AuthorCases> 'Сценарии автора' {
        private import GuideContext::*;

        use case 'Опубликовать отзыв' {
            subject guide : Guide;
            actor 'Автор' : Author;
        }
        use case 'Прикрепить фотографии' {
            subject guide : Guide;
            actor 'Автор' : Author;
        }
        use case 'Передать ссылку' {
            subject guide : Guide;
            actor 'Автор' : Author;
        }
    }
}

// Электронный путеводитель. Бизнес-цель и связанные требования.
package GuideGoals {
    requirement <goals> 'Требования к самостоятельной прогулке' {
        requirement <BR03> 'BR-03 Самостоятельная прогулка' {
            doc /* Турист планирует экскурсию по материалам издания. */
        }
        requirement <Catalog> 'REQ-FUNC-002 Смена страны';
        requirement <Context> 'REQ-INT-001 Контекст маршрута';
        requirement <ReadFailure> 'REQ-REL-001 Отказ чтения';
        dependency Catalog to BR03;
        dependency Context to BR03;
        dependency ReadFailure to BR03;
    }
}

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

// Электронный путеводитель. Публикация обзора и обработка отказа.
package ReviewFlow {
    private import ScalarValues::Boolean;

    action def <Review> 'Обмен обзором' {
        attribute canPublish : Boolean;
        first start;
        then action <draft> 'Выбрать объект и заполнить отзыв';
        then action <validate> 'Проверить текст и вложения';
        then decide;
            if canPublish then publish;
            else retain;
        action <retain> 'Оставить черновик';
        then done;
        action <publish> 'Опубликовать обзор';
        then action <share> 'Скопировать и передать ссылку';
        then action <read> 'Открыть обзор по ссылке';
        then done;
    }
}

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

// Электронный путеводитель. Распределение требований спецификации.
package GuideRequirements {
    requirement <categories> 'Спецификация: 20 требований' {
        requirement 'Бизнес: 1';
        requirement 'Функциональность: 10';
        requirement 'Интерфейс: 4';
        requirement 'Качество обслуживания: 5' {
            requirement 'Надёжность: 2';
            requirement 'Безопасность: 1';
            requirement 'Производительность: 1';
            requirement 'Наблюдаемость: 1';
        }
    }
}

// Электронный путеводитель. Отбор прогулок по стране и времени.
package DurationFilter {
    private import ScalarValues::Boolean;
    private import ScalarValues::Integer;

    action def <Filter> 'Отбор прогулок по времени' {
        in sameCountry : Boolean;
        in durationMinutes : Integer;
        in limitMinutes : Integer;
        action <match> 'Проверить страну';
        action <compare> 'Сравнить длительность с лимитом';
        first start;
        then match;
        then decide;
            if sameCountry then compare;
            else hide;
        first compare;
        then decide;
            if durationMinutes <= limitMinutes then show;
            else hide;
        action <show> 'Показать прогулку';
        then done;
        action <hide> 'Скрыть прогулку';
        then done;
    }
}
