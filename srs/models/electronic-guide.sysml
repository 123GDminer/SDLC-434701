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
        doc /* Посетитель просматривает страны, внесённые в базу. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def ViewAttractions {
        doc /* Посетитель выбирает страну и видит её достопримечательности. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }

    use case def ViewRoute {
        doc /* Посетитель видит маршрут и порядок его точек. */
        subject guide : GuideSystem;
        actor visitor : Visitor;
    }
}
