// Электронный путеводитель. Данные каталога, обзоров и вложений.
package GuideData {
    part def Country;
    part def Subject;
    part def Site :> Subject;
    part def Route :> Subject;
    part def Photograph;
    part def Review {
        ref part describing : Subject;
        part pictures : Photograph[0..*];
    }

    part <dataView> 'Связи данных' {
        part <country> 'Страна' : Country;
        part <places> 'Место' : Site[0..*];
        part <routes> 'Маршрут' : Route[0..*];
        part <reviews> 'Отзыв' : Review[0..*];
        part <photos> 'Снимок' : Photograph[0..*];
        connection 'места страны' connect country to places;
        connection 'маршруты страны' connect country to routes;
        connection 'отзыв о месте' connect reviews to places;
        connection 'отзыв о прогулке' connect reviews to routes;
        connection 'вложения' connect reviews to photos;
    }
}
