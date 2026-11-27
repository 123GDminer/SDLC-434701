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
