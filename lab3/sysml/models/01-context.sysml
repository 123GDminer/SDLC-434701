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
