package GuideReviewPhotos {
    doc /* Электронный путеводитель. Состав публикации для сценария развития из раздела 5.6. */
    part def Photograph;
    part def PlaceOrRoute;
    part def Review {
        ref part subject : PlaceOrRoute;
        part photo : Photograph[0..*];
    }
    part def ReviewEditor {
        doc /* Автор вводит текст и прикрепляет фотографии перед публикацией. */
        ref part draft : Review;
    }
    part def ReviewReader {
        doc /* Получатель ссылки открывает опубликованный отзыв вместе с его фотографиями. */
        ref part publishedReview : Review;
    }
}
