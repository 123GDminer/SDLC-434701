package ElectronicGuide {
    doc /* Предварительная модель программы «Электронный путеводитель».
           База содержит только внесённые страны, их достопримечательности
           и маршруты по достопримечательностям. */

    part def Visitor;
    part def CountryDatabase;
    part def CatalogScreen;
    part def AttractionScreen;
    part def RouteScreen;

    part def GuideSystem {
        part database : CountryDatabase;
        part catalog : CatalogScreen;
        part attractionView : AttractionScreen;
        part routeView : RouteScreen;
    }

    use case def BrowseCountries {
        doc /* Посетитель просматривает страны из базы. При ошибке загрузки получает сообщение; частичные записи не показываются как полный каталог. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def ViewAttractions {
        doc /* При выборе страны обновляются списки достопримечательностей и маршрутов. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def ViewRoute {
        doc /* Посетитель видит страну, название маршрута и порядок остановок. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }
    use case def PublishReview {
        doc /* Посетитель добавляет место или маршрут и публикует свой отзыв. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def ShareReview {
        doc /* Автор передаёт ссылку через внешний канал. Получатель открывает обзор. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def FilterByDuration {
        doc /* Турист задаёт лимит времени и видит прогулки выбранной страны в пределах лимита. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }
}
