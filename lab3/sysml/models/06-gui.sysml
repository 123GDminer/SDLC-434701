package GuideModel5 {
part def Screen;
part def GuideInterface {
 part catalog : Screen; part route : Screen; part attraction : Screen;
 part reviewEditor : Screen; part publishedReview : Screen;
 doc /* Каталог открывает маршрут; выбор остановки открывает описание места. Ссылка ведёт к опубликованному обзору. */
}
}
