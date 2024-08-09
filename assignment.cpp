#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <bitset>
#include <ctime>
#include <unordered_map>

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <conio.h>
#include <cstdlib>

char input() {
    return getch();
}

void clear() {
    system("cls");
}

#else
#include <termios.h>
#include <unistd.h>

char input() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void clear() {
    cout << "\033[2J\033[1;1H";
}

#endif

const array MONTH_NAMES = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

const array WEEKDAY_NAMES = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

enum State {
    MENU,
    ABOUT_US,
    CUSTOMER,
    STAFF,
    EXIT,
    SERVICE_LIST,
    SERVICE_1,
    SERVICE_2,
    SERVICE_3,
    EXPERT_LIST,
    BOOKING,
    BOOK_VIEW,
    BOOK_VIEW_PREV_MONTH,
    BOOK_VIEW_NEXT_MONTH,
    LOGIN,
    STAFF_MENU,
    LOGOUT,
    TREATMENT_SCHEDULE,
    CONSULTATION_SCHEDULE,
    CUSTOMER_DETAILS,
    SALES_REPORT,
};

typedef struct Service {
    string name;
    string description;
} Service;

const Service SERVICES[] = {
    {"Skin Care", "blablabla"},
    {"Face Makeup", "lorem ipsum"},
    {"Hair Coloring", "foo bar baz"},
};

typedef struct Staff {
    string name;
    string username;
    string password;
} Staff;

const array EXPERTS = {
    Staff { "Ang Zi En", "aze", "12345678" },
    Staff { "Wilson Ang Shao En", "wase", "12345678" },
    Staff { "Wai Chee Han", "wch", "12345678" },
};

const Staff ADMIN = { "Administrator", "admin", "12345678" };
const auto WORK_HOURS = 6;

static unordered_map<time_t, bitset<WORK_HOURS * EXPERTS.size()>> schedule = {};
static const Staff *login_user = nullptr;
static struct tm view_month;

typedef vector<tuple<char, string, State>> Options;

#define unpack(...) __VA_ARGS__
#define concat(x, y) x ## y
#define opts(name, init) case name: static const Options concat(name, _OPTS) = { unpack init }; options = &concat(name, _OPTS);
#define wip(name) opts(name, ({'e', "Exit", EXIT})) cout << "WIP" << endl; break;

void line() { cout << setw(20) << setfill('-') << '-' << endl; }

void time_now(struct tm *tm) {
    auto t = time(nullptr);
    *tm = *localtime(&t);
}

State ui(State state, bool &validation)
{
    struct tm t;
    
    time_now(&t);
    
    const Options *options = nullptr;
    
    clear();
    
    cout << "COMPANY LOGO" << endl;
    line();
    cout << asctime(&t);
    line();
    
    switch (state)
    {
        opts(MENU, (
            {'1', "About us", ABOUT_US},
            {'2', "I'm a customer", CUSTOMER},
            {'3', "I'm a staff", STAFF},
            {'4', "Exit", EXIT},
        ))
        cout << "Main Menu" << endl;
        break;
        
        
        opts(ABOUT_US, ({'m', "Return to main menu", MENU}))
        cout << "About Us" << endl;
        break;
        
        
        opts(CUSTOMER, (
            {'s', "View a list of available services", SERVICE_LIST},
            {'e', "View a list of available experts", EXPERT_LIST},
            {'b', "Book service", BOOKING},
            {'m', "Return to main menu", MENU},
        ))
        cout << "Customer services" << endl;
        line();
        break;
        
        
        opts(SERVICE_LIST, (
            {'1', SERVICES[0].name, SERVICE_1},
            {'2', SERVICES[1].name, SERVICE_2},
            {'3', SERVICES[2].name, SERVICE_3},
            {'x', "Cancel", CUSTOMER},
        ))
        cout << "Available services" << endl;
        line();
        break;
        
        
        #define service(service_name, index) opts(service_name, ({'b', "Book now", BOOKING}, {'s', "Go back to service list", SERVICE_LIST})) \
            cout << SERVICES[index].name << " Service" << endl; \
            line(); \
            cout << SERVICES[index].description << endl; \
            break;
        
        service(SERVICE_1, 0)
        service(SERVICE_2, 1)
        service(SERVICE_3, 2)
        
        
        opts(EXPERT_LIST, (
            {'b', "Book an appointment with our experts", BOOKING},
            {'x', "Go back", CUSTOMER},
        ))
        cout << "Our experts:" << endl;
        
        for (const auto &expert : EXPERTS) {
            cout << "- " << expert.name << endl;
        }
        break;
        
        
        case BOOKING:
        view_month = t;
        return BOOK_VIEW;
        
        
        opts(BOOK_VIEW, (
            {'a', "Previous month", BOOK_VIEW_PREV_MONTH},
            {'d', "Next month", BOOK_VIEW_NEXT_MONTH},
            {'x', "Cancel", CUSTOMER},
        ))
        
        {
            const int CELL_WIDTH = 6;
            const int LINE_WIDTH = CELL_WIDTH * WEEKDAY_NAMES.size();
            const int MY_LEN = 3 + 4 + 1;
            const int LEFT_PADDING = (LINE_WIDTH  - MY_LEN) / 2;
            const int RIGHT_PADDING = LINE_WIDTH - MY_LEN - LEFT_PADDING;
            
            cout << setw(LEFT_PADDING) << setfill(' ') << ' ' << MONTH_NAMES[view_month.tm_mon] << " " << view_month.tm_year + 1900 << setw(RIGHT_PADDING) << ' ' << endl;
            
            for (const auto &weekday : WEEKDAY_NAMES) {
                cout << left << setw(CELL_WIDTH) << weekday;
            }
            
            cout << endl;
            
            struct tm view_day = view_month;
            view_day.tm_hour = 0;
            view_day.tm_min = 0;
            view_day.tm_sec = 0;
            view_day.tm_mday = 1;
            
            time_t tt = mktime(&view_day);
            
            if (view_day.tm_wday > 0)
                cout << setw(view_day.tm_wday * CELL_WIDTH) << ' ';
            
            while (true) {
                tt = mktime(&view_day);
                
                if (view_day.tm_mon != view_month.tm_mon)
                    break;
                
                cout << setw(2) << view_day.tm_mday++ << " TS ";
                
                if (view_day.tm_wday == 6)
                    cout << endl;
            }
            
            if (view_day.tm_wday != 0)
                cout << endl;
        }
        
        break;
        
        
        case BOOK_VIEW_PREV_MONTH:
        {
            struct tm _t = t;
            
            _t.tm_mday = 1;
            _t.tm_hour = 0;
            _t.tm_min = 0;
            _t.tm_sec = 0;
            
            struct tm vm = view_month;
            
            vm.tm_mon -= 1;
            vm.tm_mday = 1;
            vm.tm_hour = 0;
            vm.tm_min = 0;
            vm.tm_sec = 0;
            
            if (mktime(&vm) >= mktime(&_t))
                view_month = vm;
        }
        return BOOK_VIEW;
        
        
        case BOOK_VIEW_NEXT_MONTH:
        view_month.tm_mon += 1;
        mktime(&view_month);
        return BOOK_VIEW;
        
        
        opts(STAFF, (
            {'l', "Login as staff", LOGIN},
            {'m', "Return to main menu", MENU},
        ))
        break;
        
        
        opts(LOGIN, ({'m', "Return to main menu", MENU}))
        
        {
            string username, password;
            
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            
            cout << endl;
            
            if (username == ADMIN.username)
                login_user = &ADMIN;
            else {
                for (const Staff &expert : EXPERTS) {
                    if (expert.username == username) {
                        login_user = &expert;
                        break;
                    }
                }
            }
            
            if (login_user == nullptr)
                cout << "Invalid username" << endl;
            else if (password == login_user->password)
                return STAFF_MENU;
            else
                cout << "Invalid password" << endl;
        }
        
        break;
        
        
        opts(STAFF_MENU, (
            {'1', "View treatment schedule", TREATMENT_SCHEDULE},
            {'2', "View consultation schedule", CONSULTATION_SCHEDULE},
            {'3', "View customers", CUSTOMER_DETAILS},
            {'4', "View sales report", SALES_REPORT},
            {'5', "Log out", LOGOUT},
        ))
        cout << "Staff menu" << endl;
        cout << "Currently login as " << login_user->username << " (" << login_user->name << ")" << endl;
        break;
        
        
        wip(TREATMENT_SCHEDULE)
        
        
        wip(CONSULTATION_SCHEDULE)
        
        
        wip(CUSTOMER_DETAILS)
        
        
        wip(SALES_REPORT)
        
        
        opts(LOGOUT, (
            {'l', "Log in", LOGIN},
            {'m', "Return to main menu", MENU},
        ))
        login_user = nullptr;
        cout << "Successfully logged out!" << endl;
        break;
        
        
    case EXIT:
        cout << "Thank you for using our service" << endl;
        return EXIT;
        
    default:
        break;
    }
    
    cout << endl;
    
    if (!validation)
        cout << "Invalid option" << endl << endl;
    
    validation = true;
    
    if (options->size() > 1) {
        cout << "Please select the options" << endl;
        
        for (const auto &opt : *options) {
            cout << get<0>(opt) << ": " << get<1>(opt) << endl;
        }
    }
    else
        cout << "Press " << get<0>((*options)[0]) << " to " << get<1>((*options)[0]) << endl;
    
    char c;
    
    while ((c = input()) == '\n');
    
    for (const auto &opt : *options) {
        if (get<0>(opt) == c) {
            return get<2>(opt);
        }
    }
    
    validation = false;
    
    return state;
}

int main()
{
    bool validation = true;
    State state = ui(MENU, validation);
    
    while (state != EXIT) {
        state = ui(state, validation);
    }
    
    ui(state, validation);
    
    return 0;
}
