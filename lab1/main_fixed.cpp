#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <functional>
#include <stdexcept>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// =====================================================
// MODEL   
// =====================================================

class Model
{
private:
    int age = 0;
    long long result = 0;

    //   :
    // 2       " ".
    static constexpr int POSTPONEMENTS_PER_DAY = 2;

    std::function<void()> onChanged;

public:
    void setOnChanged(std::function<void()> callback)
    {
        onChanged = callback;
    }

    bool setAge(int newAge)
    {
        if (newAge < 1 || newAge > 120)
            return false;

        age = newAge;

        result =
            static_cast<long long>(age) *
            365LL *
            POSTPONEMENTS_PER_DAY;

        //     .
        if (onChanged)
            onChanged();

        return true;
    }

    int getAge() const
    {
        return age;
    }

    long long getResult() const
    {
        return result;
    }
};

// =====================================================
// VIEW   
// =====================================================

class MainView
{
private:
    HWND hwnd = nullptr;
    HWND buttonInput = nullptr;
    HWND labelData = nullptr;
    HWND labelResult = nullptr;

public:
    void setWindow(HWND window)
    {
        hwnd = window;
    }

    HWND getWindow() const
    {
        return hwnd;
    }

    void createControls()
    {
        buttonInput = CreateWindowW(
            L"BUTTON",
            L"\u0412\u0432\u0435\u0441\u0442\u0438 \u0434\u0430\u043d\u043d\u044b\u0435",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            30, 30, 180, 40,
            hwnd,
            reinterpret_cast<HMENU>(1001),
            GetModuleHandleW(nullptr),
            nullptr
        );

        labelData = CreateWindowW(
            L"STATIC",
            L"\u041f\u043e\u0441\u043b\u0435\u0434\u043d\u0438\u0435 \u0434\u0430\u043d\u043d\u044b\u0435: \u043d\u0435\u0442",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            30, 90, 520, 25,
            hwnd,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );

        labelResult = CreateWindowW(
            L"STATIC",
            L"\u0420\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442: \u043d\u0435\u0442",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            30, 125, 520, 45,
            hwnd,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );
    }

    void updateData(int age)
    {
        std::wstring text =
            L"\u041f\u043e\u0441\u043b\u0435\u0434\u043d\u0438\u0435 \u0434\u0430\u043d\u043d\u044b\u0435: \u0432\u043e\u0437\u0440\u0430\u0441\u0442 = " +
            std::to_wstring(age);

        SetWindowTextW(labelData, text.c_str());
    }

    void updateResult(long long result)
    {
        std::wstring text =
            L"\u041f\u0440\u0438\u043c\u0435\u0440\u043d\u043e\u0435 \u043a\u043e\u043b\u0438\u0447\u0435\u0441\u0442\u0432\u043e \u0440\u0430\u0437, \u043a\u043e\u0433\u0434\u0430 \u0447\u0435\u043b\u043e\u0432\u0435\u043a \u0441\u043a\u0430\u0437\u0430\u043b \u00ab\u043f\u043e\u0442\u043e\u043c\u00bb: " +
            std::to_wstring(result);

        SetWindowTextW(labelResult, text.c_str());
    }

    void showError(const std::wstring& message)
    {
        MessageBoxW(
            hwnd,
            message.c_str(),
            L"\u041e\u0448\u0438\u0431\u043a\u0430",
            MB_OK | MB_ICONERROR
        );
    }
};

// =====================================================
// INPUT VIEW   
// =====================================================

class InputWindow
{
private:
    HWND hwnd = nullptr;
    HWND editAge = nullptr;
    HWND parent = nullptr;

    std::function<void(int)> onOk;

public:
    InputWindow(
        HWND parentWindow,
        int lastAge,
        std::function<void(int)> callback)
        : parent(parentWindow), onOk(callback)
    {
        WNDCLASSW wc{};

        wc.lpfnWndProc = InputWindow::WndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"InputWindowClass";
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        //      .
        RegisterClassW(&wc);

        hwnd = CreateWindowW(
            L"InputWindowClass",
            L"\u0412\u0432\u043e\u0434 \u0434\u0430\u043d\u043d\u044b\u0445",
            WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            350,
            220,
            parent,
            nullptr,
            GetModuleHandleW(nullptr),
            this
        );

        if (hwnd == nullptr)
        {
            return;
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        //    .
        if (lastAge > 0 && editAge != nullptr)
        {
            SetWindowTextW(
                editAge,
                std::to_wstring(lastAge).c_str()
            );

            //    .
            SendMessageW(editAge, EM_SETSEL, 0, -1);
            SetFocus(editAge);
        }
    }

    HWND getWindow() const
    {
        return hwnd;
    }

    static LRESULT CALLBACK WndProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam)
    {
        InputWindow* window =
            reinterpret_cast<InputWindow*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA)
            );

        if (message == WM_NCCREATE)
        {
            auto* createStruct =
                reinterpret_cast<CREATESTRUCTW*>(lParam);

            window =
                reinterpret_cast<InputWindow*>(
                    createStruct->lpCreateParams
                );

            SetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(window)
            );
        }

        if (window == nullptr)
        {
            return DefWindowProcW(
                hwnd, message, wParam, lParam
            );
        }

        switch (message)
        {
        case WM_CREATE:
        {
            CreateWindowW(
                L"STATIC",
                L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0432\u043e\u0437\u0440\u0430\u0441\u0442:",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                30, 25, 200, 25,
                hwnd,
                nullptr,
                GetModuleHandleW(nullptr),
                nullptr
            );

            window->editAge = CreateWindowW(
                L"EDIT",
                L"",
                WS_VISIBLE | WS_CHILD |
                WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
                30, 55, 270, 30,
                hwnd,
                reinterpret_cast<HMENU>(2001),
                GetModuleHandleW(nullptr),
                nullptr
            );

            CreateWindowW(
                L"BUTTON",
                L"OK",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                70, 105, 90, 35,
                hwnd,
                reinterpret_cast<HMENU>(2002),
                GetModuleHandleW(nullptr),
                nullptr
            );

            CreateWindowW(
                L"BUTTON",
                L"\u041e\u0442\u043c\u0435\u043d\u0430",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                180, 105, 90, 35,
                hwnd,
                reinterpret_cast<HMENU>(2003),
                GetModuleHandleW(nullptr),
                nullptr
            );

            return 0;
        }

        case WM_COMMAND:
        {
            const int id = LOWORD(wParam);

            if (id == 2002)
            {
                wchar_t buffer[100]{};

                GetWindowTextW(
                    window->editAge,
                    buffer,
                    100
                );

                std::wstring input(buffer);

                if (input.empty())
                {
                    MessageBoxW(
                        hwnd,
                        L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0432\u043e\u0437\u0440\u0430\u0441\u0442.",
                        L"\u041e\u0448\u0438\u0431\u043a\u0430",
                        MB_OK | MB_ICONERROR
                    );

                    SetFocus(window->editAge);
                    return 0;
                }

                // ,    .
                if (input.find_first_not_of(L"0123456789") != std::wstring::npos)
                {
                    MessageBoxW(
                        hwnd,
                        L"\u0412\u043e\u0437\u0440\u0430\u0441\u0442 \u0434\u043e\u043b\u0436\u0435\u043d \u0431\u044b\u0442\u044c \u0446\u0435\u043b\u044b\u043c \u0447\u0438\u0441\u043b\u043e\u043c.",
                        L"\u041e\u0448\u0438\u0431\u043a\u0430",
                        MB_OK | MB_ICONERROR
                    );

                    SetFocus(window->editAge);
                    return 0;
                }

                int age = 0;

                try
                {
                    size_t processed = 0;
                    age = std::stoi(input, &processed);

                    if (processed != input.size())
                    {
                        throw std::invalid_argument("invalid input");
                    }
                }
                catch (...)
                {
                    MessageBoxW(
                        hwnd,
                        L"\u041d\u0435\u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u044b\u0435 \u0434\u0430\u043d\u043d\u044b\u0435.",
                        L"\u041e\u0448\u0438\u0431\u043a\u0430",
                        MB_OK | MB_ICONERROR
                    );

                    SetFocus(window->editAge);
                    return 0;
                }

                if (age < 1 || age > 120)
                {
                    MessageBoxW(
                        hwnd,
                        L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u0432\u043e\u0437\u0440\u0430\u0441\u0442 \u043e\u0442 1 \u0434\u043e 120 \u043b\u0435\u0442.",
                        L"\u041e\u0448\u0438\u0431\u043a\u0430",
                        MB_OK | MB_ICONERROR
                    );

                    SetFocus(window->editAge);
                    return 0;
                }

                if (window->onOk)
                {
                    window->onOk(age);
                }

                DestroyWindow(hwnd);
                return 0;
            }

            if (id == 2003)
            {
                DestroyWindow(hwnd);
                return 0;
            }

            break;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY:
        {
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            LRESULT result = DefWindowProcW(
                hwnd, message, wParam, lParam
            );
            delete window;
            return result;
        }
        }

        return DefWindowProcW(
            hwnd, message, wParam, lParam
        );
    }
};

// =====================================================
// CONTROLLER
// =====================================================

class Controller
{
private:
    Model model;
    MainView view;

    //   .
    int lastAge = 0;

public:
    void initialize(HWND hwnd)
    {
        view.setWindow(hwnd);
        view.createControls();

        //     View.
        model.setOnChanged([this]()
        {
            view.updateData(model.getAge());
            view.updateResult(model.getResult());
        });
    }

    void openInputWindow()
    {
        new InputWindow(
            view.getWindow(),
            lastAge,

            [this](int age)
            {
                if (!model.setAge(age))
                {
                    view.showError(
                        L"\u041d\u0435\u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u044b\u0435 \u0434\u0430\u043d\u043d\u044b\u0435."
                    );
                    return;
                }

                lastAge = age;
            }
        );
    }
};

// =====================================================
//   
// =====================================================

Controller* controller = nullptr;

LRESULT CALLBACK MainWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        controller = new Controller();
        controller->initialize(hwnd);
        return 0;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1001)
        {
            controller->openInputWindow();
            return 0;
        }

        break;
    }

    case WM_DESTROY:
    {
        delete controller;
        controller = nullptr;

        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(
        hwnd, message, wParam, lParam
    );
}

// =====================================================
//  
// =====================================================

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    PWSTR,
    int nCmdShow)
{
    WNDCLASSW wc{};

    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainWindowClass";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(
            nullptr,
            L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u0437\u0430\u0440\u0435\u0433\u0438\u0441\u0442\u0440\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u043e\u043a\u043d\u043e.",
            L"\u041e\u0448\u0438\u0431\u043a\u0430",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    HWND hwnd = CreateWindowW(
        L"MainWindowClass",
        L"\u0421\u043a\u043e\u043b\u044c\u043a\u043e \u0440\u0430\u0437 \u0442\u044b \u0441\u043a\u0430\u0437\u0430\u043b \u00ab\u043f\u043e\u0442\u043e\u043c\u00bb",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        620,
        280,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr)
    {
        MessageBoxW(
            nullptr,
            L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u0441\u043e\u0437\u0434\u0430\u0442\u044c \u0433\u043b\u0430\u0432\u043d\u043e\u0435 \u043e\u043a\u043d\u043e.",
            L"\u041e\u0448\u0438\u0431\u043a\u0430",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG message{};

    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
