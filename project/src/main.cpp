// Электронный путеводитель. Окно каталога, карточек и маршрутов.
#include "catalog.hpp"

#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>

#include <algorithm>
#include <optional>
#include <stdexcept>

namespace {
constexpr wchar_t windowClass[] = L"ElectronicGuideWindow";
constexpr COLORREF backgroundColor = RGB(244, 247, 246);
constexpr COLORREF textColor = RGB(22, 43, 40);
constexpr COLORREF headerColor = RGB(22, 62, 55);

enum ControlId { Search = 101, Countries, Attractions, Routes, Stops };

std::filesystem::path databasePath()
{
    std::wstring path(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (length == 0 || length == path.size())
        throw std::runtime_error("Executable path is unavailable");
    path.resize(length);
    return std::filesystem::path(path).parent_path() / L"catalog.sqlite3";
}

std::wstring windowText(HWND control)
{
    const int capacity = GetWindowTextLengthW(control) + 1;
    std::wstring text(capacity, L'\0');
    text.resize(GetWindowTextW(control, text.data(), capacity));
    return text;
}

void setText(HWND control, const std::wstring& text)
{
    SetWindowTextW(control, text.c_str());
}

void addRow(HWND list, const std::wstring& text)
{
    SendMessageW(list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
}

std::optional<std::size_t> selection(HWND list)
{
    const auto index = SendMessageW(list, LB_GETCURSEL, 0, 0);
    return index == LB_ERR ? std::nullopt : std::optional{static_cast<std::size_t>(index)};
}

std::wstring attractionLabel(const Attraction& attraction)
{
    return attraction.city.empty() ? attraction.name : attraction.name + L" · " + attraction.city;
}

class GuideWindow {
public:
    explicit GuideWindow(HINSTANCE instance) : instance_(instance) {}

    ~GuideWindow()
    {
        DeleteObject(bodyFont_);
        DeleteObject(titleFont_);
        DeleteObject(headingFont_);
        DeleteObject(backgroundBrush_);
        DeleteObject(headerBrush_);
    }

    bool open(int showCommand)
    {
        WNDCLASSW definition{};
        definition.lpfnWndProc = dispatch;
        definition.hInstance = instance_;
        definition.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        definition.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        definition.lpszClassName = windowClass;
        if (!RegisterClassW(&definition))
            return false;

        window_ = CreateWindowExW(0, windowClass, L"Электронный путеводитель",
                                  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                  CW_USEDEFAULT, CW_USEDEFAULT, px(1100), px(760),
                                  nullptr, nullptr, instance_, this);
        if (!window_)
            return false;
        ShowWindow(window_, showCommand);
        return true;
    }

    int run() const
    {
        MSG message{};
        BOOL result;
        while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
            if (!IsDialogMessageW(window_, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        return result == -1 ? 1 : static_cast<int>(message.wParam);
    }

private:
    static LRESULT CALLBACK dispatch(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_NCCREATE) {
            const auto* creation = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto* self = static_cast<GuideWindow*>(creation->lpCreateParams);
            self->window_ = window;
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        auto* self = reinterpret_cast<GuideWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        return self ? self->handle(message, wParam, lParam)
                    : DefWindowProcW(window, message, wParam, lParam);
    }

    int px(int size) const { return MulDiv(size, dpi_, 96); }

    HFONT font(int size, int weight) const
    {
        return CreateFontW(-px(size), 0, 0, 0, weight, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    }

    HWND control(const wchar_t* type, const wchar_t* text, DWORD style, HFONT face,
                 int id = 0, DWORD extra = 0) const
    {
        HWND child = CreateWindowExW(extra, type, text, WS_CHILD | WS_VISIBLE | style,
                                     0, 0, 0, 0, window_,
                                     reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                                     instance_, nullptr);
        SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(face), FALSE);
        return child;
    }

    HWND label(const wchar_t* text, HFONT face, DWORD style = SS_NOPREFIX) const
    {
        return control(L"STATIC", text, style, face);
    }

    HWND list(ControlId id) const
    {
        const DWORD routeStyle = id == Stops ? LBS_OWNERDRAWFIXED | LBS_HASSTRINGS : 0;
        const auto child = control(L"LISTBOX", L"",
            WS_TABSTOP | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | routeStyle,
            bodyFont_, id, WS_EX_CLIENTEDGE);
        SetWindowTheme(child, L"Explorer", nullptr);
        return child;
    }

    void createControls()
    {
        bodyFont_ = font(17, FW_NORMAL);
        headingFont_ = font(19, FW_SEMIBOLD);
        titleFont_ = font(27, FW_SEMIBOLD);
        backgroundBrush_ = CreateSolidBrush(backgroundColor);
        headerBrush_ = CreateSolidBrush(headerColor);

        title_ = label(L"Электронный путеводитель", titleFont_);
        subtitle_ = label(L"Страны, достопримечательности и экскурсионные маршруты", bodyFont_);
        countriesLabel_ = label(L"Страны в базе", headingFont_);
        searchLabel_ = label(L"Поиск по названию", bodyFont_);
        search_ = control(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, bodyFont_, Search, WS_EX_CLIENTEDGE);
        SendMessageW(search_, EM_LIMITTEXT, 128, 0);
        countries_ = list(Countries);
        countryTitle_ = label(L"", titleFont_);
        countryDescription_ = label(L"", bodyFont_);
        attractionsLabel_ = label(L"", headingFont_);
        attractions_ = list(Attractions);
        routesLabel_ = label(L"", headingFont_);
        routes_ = list(Routes);
        detailTitle_ = label(L"", headingFont_);
        detail_ = control(L"EDIT", L"", WS_TABSTOP | WS_VSCROLL | ES_MULTILINE
            | ES_READONLY | ES_AUTOVSCROLL, bodyFont_, 0, WS_EX_CLIENTEDGE);
        stopsLabel_ = label(L"Порядок посещения", headingFont_);
        stops_ = list(Stops);
        showStops(false);

        try {
            catalog_ = loadCatalog(databasePath());
        } catch (const std::exception&) {
            databaseUnavailable_ = true;
        }
        filterCountries();
    }

    void place(HWND control, int x, int y, int width, int height) const
    {
        MoveWindow(control, x, y, std::max(0, width), std::max(0, height), TRUE);
    }

    void layout() const
    {
        if (!detail_)
            return;
        RECT client{};
        GetClientRect(window_, &client);
        const int margin = px(24);
        const int gap = px(20);
        const int width = client.right - 2 * margin;
        const int sidebar = px(235);
        const int contentX = margin + sidebar + gap;
        const int contentWidth = width - sidebar - gap;
        const int column = (contentWidth - gap) / 2;
        const int top = px(112);
        const int listTop = top + px(108);
        const int clientHeight = static_cast<int>(client.bottom);
        const int detailTop = listTop + std::max(px(100), (clientHeight - listTop - margin) / 2);

        place(title_, margin, px(17), width, px(35));
        place(subtitle_, margin, px(55), width, px(24));
        place(countriesLabel_, margin, top, sidebar, px(26));
        place(searchLabel_, margin, top + px(34), sidebar, px(20));
        place(search_, margin, top + px(57), sidebar, px(32));
        place(countries_, margin, top + px(100), sidebar, client.bottom - top - px(100) - margin);
        place(countryTitle_, contentX, top, contentWidth, px(36));
        place(countryDescription_, contentX, top + px(40), contentWidth, px(41));
        place(attractionsLabel_, contentX, listTop - px(27), column, px(25));
        place(routesLabel_, contentX + column + gap, listTop - px(27), contentWidth - column - gap, px(25));
        place(attractions_, contentX, listTop, column, detailTop - listTop - gap);
        place(routes_, contentX + column + gap, listTop, contentWidth - column - gap, detailTop - listTop - gap);
        place(detailTitle_, contentX, detailTop, contentWidth, px(28));
        const int detailsBottom = client.bottom - detailTop - margin;
        place(detail_, contentX, detailTop + px(34),
              showingStops_ ? column : contentWidth, detailsBottom - px(34));
        place(stopsLabel_, contentX + column + gap, detailTop + px(32), contentWidth - column - gap, px(24));
        place(stops_, contentX + column + gap, detailTop + px(59), contentWidth - column - gap, detailsBottom - px(59));
    }

    void filterCountries()
    {
        const auto previous = countryIndex_;
        const auto phrase = windowText(search_);
        visibleCountries_.clear();
        SendMessageW(countries_, LB_RESETCONTENT, 0, 0);

        for (std::size_t index = 0; index < catalog_.size(); ++index) {
            const auto& name = catalog_[index].name;
            if (phrase.empty() || FindNLSStringEx(L"ru-RU", FIND_FROMSTART | NORM_IGNORECASE,
                name.c_str(), -1, phrase.c_str(), -1, nullptr, nullptr, nullptr, 0) >= 0) {
                visibleCountries_.push_back(index);
                addRow(countries_, name);
            }
        }

        if (visibleCountries_.empty()) {
            displayCountry(std::nullopt);
            return;
        }
        const auto preserved = previous
            ? std::find(visibleCountries_.begin(), visibleCountries_.end(), *previous)
            : visibleCountries_.end();
        const auto row = preserved == visibleCountries_.end() ? 0 : preserved - visibleCountries_.begin();
        SendMessageW(countries_, LB_SETCURSEL, row, 0);
        if (countryIndex_ != visibleCountries_[row])
            displayCountry(visibleCountries_[row]);
    }

    const Country& country() const { return catalog_[*countryIndex_]; }

    void displayCountry(std::optional<std::size_t> index)
    {
        countryIndex_ = index;
        routeIndex_.reset();
        SendMessageW(attractions_, LB_RESETCONTENT, 0, 0);
        SendMessageW(routes_, LB_RESETCONTENT, 0, 0);
        showStops(false);
        setText(attractionsLabel_, L"Достопримечательности");
        setText(routesLabel_, L"Маршруты");
        setText(detailTitle_, L"Сведения");
        EnableWindow(attractions_, index && !country().attractions.empty());
        EnableWindow(routes_, index && !country().routes.empty());

        if (!index) {
            setText(countryTitle_, databaseUnavailable_ ? L"База недоступна" :
                    catalog_.empty() ? L"В базе пока нет стран" : L"Страны не найдены");
            setText(countryDescription_, L"");
            setText(detail_, databaseUnavailable_
                ? L"Не удалось прочитать данные. Проверьте файл catalog.sqlite3 рядом с программой."
                : catalog_.empty() ? L"Добавьте страны в каталог." : L"Измените или очистите поисковый запрос.");
            EnableWindow(search_, !databaseUnavailable_ && !catalog_.empty());
            EnableWindow(countries_, FALSE);
            return;
        }

        EnableWindow(countries_, TRUE);
        setText(countryTitle_, country().name);
        setText(countryDescription_, country().description);
        for (const auto& attraction : country().attractions)
            addRow(attractions_, attractionLabel(attraction));
        for (const auto& route : country().routes)
            addRow(routes_, route.name);

        std::wstring details;
        if (country().attractions.empty())
            details += L"Достопримечательности для этой страны пока не внесены.\r\n";
        if (country().routes.empty())
            details += L"Маршруты для этой страны пока не внесены.\r\n";
        if (!country().attractions.empty() || !country().routes.empty())
            details += L"Выберите достопримечательность или маршрут, чтобы посмотреть сведения.";
        setText(detail_, details);
    }

    void showStops(bool show)
    {
        showingStops_ = show;
        ShowWindow(stopsLabel_, show ? SW_SHOW : SW_HIDE);
        ShowWindow(stops_, show ? SW_SHOW : SW_HIDE);
        if (!show)
            SendMessageW(stops_, LB_RESETCONTENT, 0, 0);
        layout();
    }

    void showAttraction(std::size_t index)
    {
        routeIndex_.reset();
        SendMessageW(routes_, LB_SETCURSEL, static_cast<WPARAM>(-1), 0);
        showStops(false);
        const auto& attraction = country().attractions.at(index);
        setText(detailTitle_, attractionLabel(attraction));
        setText(detail_, attraction.description.empty() ? L"Описание пока отсутствует." : attraction.description);
    }

    const Attraction& routeStop(RecordId id) const
    {
        const auto& attractions = country().attractions;
        return *std::find_if(attractions.begin(), attractions.end(), [id](const Attraction& attraction) {
            return attraction.id == id;
        });
    }

    void showRoute(std::size_t index)
    {
        routeIndex_ = index;
        SendMessageW(attractions_, LB_SETCURSEL, static_cast<WPARAM>(-1), 0);
        SendMessageW(stops_, LB_RESETCONTENT, 0, 0);
        const auto& route = country().routes.at(index);
        for (std::size_t row = 0; row < route.stops.size(); ++row) {
            addRow(stops_, std::to_wstring(row + 1) + L". " + routeStop(route.stops[row]).name);
        }
        setText(detailTitle_, route.name);
        const auto summary = route.description.empty() ? L"" : route.description + L"\r\n\r\n";
        setText(detail_, country().name + L"\r\nТочек маршрута: " + std::to_wstring(route.stops.size())
                + L"\r\n\r\n" + summary + L"Выберите остановку, чтобы открыть её описание.");
        showStops(true);
    }

    void showRouteStop(std::size_t row)
    {
        const auto& attraction = routeStop(country().routes.at(*routeIndex_).stops.at(row));
        setText(detail_, country().name + L"\r\n" + std::to_wstring(row + 1) + L". "
                + attractionLabel(attraction) + L"\r\n\r\n"
                + (attraction.description.empty() ? L"Описание пока отсутствует." : attraction.description));
    }

    void paintStop(const DRAWITEMSTRUCT& item) const
    {
        const bool selected = (item.itemState & ODS_SELECTED) != 0;
        FillRect(item.hDC, &item.rcItem, GetSysColorBrush(selected ? COLOR_HIGHLIGHT : COLOR_WINDOW));
        if (item.itemID == static_cast<UINT>(-1) || !routeIndex_)
            return;
        const auto& route = country().routes.at(*routeIndex_);
        if (item.itemID >= route.stops.size())
            return;

        const int saved = SaveDC(item.hDC);
        const auto ink = selected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : headerColor;
        const auto centerX = item.rcItem.left + px(21);
        const auto centerY = (item.rcItem.top + item.rcItem.bottom) / 2;
        const auto radius = px(13);
        SelectObject(item.hDC, GetStockObject(DC_PEN));
        SelectObject(item.hDC, GetStockObject(DC_BRUSH));
        SetDCPenColor(item.hDC, ink);
        SetDCBrushColor(item.hDC, selected ? GetSysColor(COLOR_HIGHLIGHT) : RGB(225, 239, 233));

        const auto start = item.itemID == 0 ? centerY : item.rcItem.top;
        const auto end = item.itemID + 1 == route.stops.size() ? centerY : item.rcItem.bottom;
        MoveToEx(item.hDC, centerX, start, nullptr);
        LineTo(item.hDC, centerX, end);
        Ellipse(item.hDC, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

        SetBkMode(item.hDC, TRANSPARENT);
        SetTextColor(item.hDC, ink);
        SelectObject(item.hDC, bodyFont_);
        RECT numberBounds{centerX - radius, item.rcItem.top, centerX + radius, item.rcItem.bottom};
        const auto number = std::to_wstring(item.itemID + 1);
        DrawTextW(item.hDC, number.c_str(), -1, &numberBounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        RECT titleBounds = item.rcItem;
        titleBounds.left += px(44);
        titleBounds.right -= px(6);
        const auto& name = routeStop(route.stops[item.itemID]).name;
        DrawTextW(item.hDC, name.c_str(), -1, &titleBounds,
                  DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        if (item.itemState & ODS_FOCUS)
            DrawFocusRect(item.hDC, &item.rcItem);
        RestoreDC(item.hDC, saved);
    }

    LRESULT handle(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message) {
        case WM_CREATE:
            createControls();
            return 0;
        case WM_SIZE:
            layout();
            return 0;
        case WM_GETMINMAXINFO: {
            auto* limits = reinterpret_cast<MINMAXINFO*>(lParam);
            limits->ptMinTrackSize = {px(890), px(640)};
            return 0;
        }
        case WM_MEASUREITEM:
            if (wParam == Stops) {
                auto* item = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
                item->itemHeight = static_cast<UINT>(px(34));
                return TRUE;
            }
            break;
        case WM_DRAWITEM:
            if (wParam == Stops) {
                paintStop(*reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
                return TRUE;
            }
            break;
        case WM_COMMAND: {
            const int id = LOWORD(wParam);
            const int notification = HIWORD(wParam);
            if (id == Search && notification == EN_CHANGE && detail_)
                filterCountries();
            if (notification != LBN_SELCHANGE)
                return 0;
            if (id == Countries) {
                if (const auto row = selection(countries_))
                    displayCountry(visibleCountries_.at(*row));
            } else if (id == Attractions && countryIndex_) {
                if (const auto row = selection(attractions_))
                    showAttraction(*row);
            } else if (id == Routes && countryIndex_) {
                if (const auto row = selection(routes_))
                    showRoute(*row);
            } else if (id == Stops && routeIndex_) {
                if (const auto row = selection(stops_))
                    showRouteStop(*row);
            }
            return 0;
        }
        case WM_ERASEBKGND: {
            const auto canvas = reinterpret_cast<HDC>(wParam);
            RECT bounds{};
            GetClientRect(window_, &bounds);
            FillRect(canvas, &bounds, backgroundBrush_);
            bounds.bottom = px(90);
            FillRect(canvas, &bounds, headerBrush_);
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            const auto canvas = reinterpret_cast<HDC>(wParam);
            const auto child = reinterpret_cast<HWND>(lParam);
            const bool header = child == title_ || child == subtitle_;
            SetTextColor(canvas, header ? RGB(255, 255, 255) : textColor);
            SetBkColor(canvas, header ? headerColor : backgroundColor);
            return reinterpret_cast<LRESULT>(header ? headerBrush_ : backgroundBrush_);
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
        }
        return DefWindowProcW(window_, message, wParam, lParam);
    }

    HINSTANCE instance_;
    int dpi_ = static_cast<int>(GetDpiForSystem());
    HWND window_{}, title_{}, subtitle_{}, countriesLabel_{}, searchLabel_{}, search_{}, countries_{};
    HWND countryTitle_{}, countryDescription_{}, attractionsLabel_{}, attractions_{};
    HWND routesLabel_{}, routes_{}, detailTitle_{}, detail_{}, stopsLabel_{}, stops_{};
    HFONT bodyFont_{}, headingFont_{}, titleFont_{};
    HBRUSH backgroundBrush_{}, headerBrush_{};
    std::vector<Country> catalog_;
    std::vector<std::size_t> visibleCountries_;
    std::optional<std::size_t> countryIndex_, routeIndex_;
    bool showingStops_ = false;
    bool databaseUnavailable_ = false;
};
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    SetProcessDPIAware();
    InitCommonControls();
    GuideWindow guide(instance);
    if (!guide.open(showCommand)) {
        MessageBoxW(nullptr, L"Не удалось открыть окно программы.", L"Электронный путеводитель", MB_ICONERROR);
        return 1;
    }
    return guide.run();
}
