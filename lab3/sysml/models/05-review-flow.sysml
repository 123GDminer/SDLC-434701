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
