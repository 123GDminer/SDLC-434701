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
