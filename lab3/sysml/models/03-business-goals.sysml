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
