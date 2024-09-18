#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

void clear() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    system("cls");
#else
    cout << "\E[H\E[2J";
#endif
}

#define DAY_COUNT 30
#define EXPERT_COUNT 3
#define SERVICE_COUNT 3
#define WORK_HOUR_COUNT 6
#define OPENING_HOUR 12
#define CUSTOMER_ENTRIES_PER_PAGE 10

enum Gender : bool {
    MALE,
    FEMALE,
};

enum ServiceType : bool {
    TREATMENT,
    CONSULTATION,
};

enum State {
    MAIN_MENU,
    
    
    ABOUT_US,
    
    
    CUSTOMER_LOGIN,
    CUSTOMER_MENU,
    SERVICE_LIST,
    VIEW_SERVICE,
    EXPERT_LIST,
    APPOINTMENTS,
    
    BOOKING,
    BOOK_SELECT_DAY,
    BOOK_SELECT_SERVICE_TYPE,
    BOOK_TREATMENT,
    BOOK_CONSULTATION,
    BOOK_SELECT_EXPERT,
    BOOK_SELECT_SERVICE,
    BOOK_CONFIRM,
    BOOK_PAYMENT,
    BOOK_SUCCESS,
    BOOK_CANCEL,
    
    
    STAFF_LOGIN,
    STAFF_MENU,
    INVALID_USER,
    
    TREATMENT_SCHEDULE,
    CONSULTATION_SCHEDULE,
    SCHEDULE_VIEW,
    SCHEDULE_VIEW_PREV_WEEK,
    SCHEDULE_VIEW_NEXT_WEEK,
    CUSTOMER_DETAILS,
    CUSTOMER_VIEW,
    CUSTOMER_VIEW_PREV_PAGE,
    CUSTOMER_VIEW_NEXT_PAGE,
    
    SALES_REPORT,
    SALES_REPORT_CUSTOMER_SUMMARY,
    SALES_REPORT_EXPERT_SUMMARY,
    SALES_REPORT_FINANCE_SUMMARY,
    
    
    LOGOUT,
    
    EXIT,
};

struct Service {
    string name;
    string description;
    double consultation_fee;
    double treatment_fee;
};

struct User {
    string name;
    Gender gender;
    string phone_number;
    string username;
    string password;
};

struct ExpertSchedule {
    string treatment_schedule[DAY_COUNT][WORK_HOUR_COUNT];
    string consultation_schedule[DAY_COUNT][WORK_HOUR_COUNT];
};

struct Booking {
    ServiceType service_type;
    int service;
    int expert;
    int day;
    int hour;
    string customer_username;
};

struct Cache {
    bool schedule[DAY_COUNT][WORK_HOUR_COUNT][EXPERT_COUNT];
    ExpertSchedule experts[EXPERT_COUNT];
    User user;
    Booking booking;
    int new_booking_id;
    int schedule_view_week;
    int customer_count;
    int customer_view_page;
};

struct Option {
    string description;
    State link;
};

const struct {
    string CUSTOMER = "customer.dat";
    string BOOKING = "booking.dat";
    string CACHE = "cache.dat";
} FILES;

const string DAYS_OF_A_WEEK[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
};

const string SHORT_DAYS_OF_A_WEEK[] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

const Service SERVICES[] = {
    {"Skin Care", "blablabla", 100, 200},
    {"Face Makeup", "lorem ipsum", 123, 456},
    {"Hair Coloring", "foo bar baz", 999, 9999},
};

const User EXPERTS[] = {
    { "Ang Zi En", FEMALE, "999", "aze", "12345678" },
    { "Wai Chee Han", MALE, "999", "wch", "12345678" },
    { "Wilson Ang Shao En", MALE, "+6011-7243 3165", "wase", "12345678" },
};

const User ADMIN = { "Administrator", MALE, "-", "admin", "12345678" };

void coutfill(char c, int n) {
    if (n > 0)
        cout << setfill(c) << setw(n) << c;
}

void line() {
    coutfill('-', 20);
    cout << endl;
}

ostream &center(int target_length, int string_length, const string &s) {
    if (string_length > target_length)
        cout << s;
    else {
        int padding = target_length - string_length;
        int left = padding / 2;
        int right = padding - left;
        
        coutfill(' ', left);
        
        cout << s;
        
        coutfill(' ', right);
    }
    
    return cout;
}

int find_longest_expert_name_length() {
    int length = 0;
    
    for (const User &expert : EXPERTS)
        if (expert.name.length() > length)
            length = expert.name.length();
    
    return length;
}

string format_date(int day) {
    return DAYS_OF_A_WEEK[day % 7] + " " + to_string(day + 1) + " September 2024";
}

string format_time(int day, int hour) {
    return format_date(day) + " " + to_string(hour + OPENING_HOUR) + ":00";
}

string format_booking_time(const Booking &booking) {
    return format_time(booking.day, booking.hour) + "~" + to_string(booking.hour + OPENING_HOUR + (booking.service_type == TREATMENT ? 2 : 1)) + ":00";
}

bool validate_username(const string &s) {
    for (char c : s)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
            return false;
    
    return true;
}

bool check_availability(const Cache &cache, ServiceType type, int day, int hour, int expert) {
    if (day == -1) {
        for (int i = 0; i < DAY_COUNT; i++)
            if (check_availability(cache, type, i, hour, expert))
                return true;
        
        return false;
    }
    
    if (hour == -1) {
        for (int i = 0; i < WORK_HOUR_COUNT - (type == TREATMENT ? 1 : 0); i++)
            if (check_availability(cache, type, day, i, expert))
                return true;
        
        return false;
    }
    
    if (expert == -1) {
        for (int i = 0; i < EXPERT_COUNT; i++)
            if (check_availability(cache, type, day, hour, i))
                return true;
        
        return false;
    }
    
    switch (type) {
        case TREATMENT:
            return !(cache.schedule[day][hour][expert] || cache.schedule[day][hour + 1][expert]);
        
        case CONSULTATION:
            return !cache.schedule[day][hour][expert];
    }
}

int read_int(ifstream &in) {
    string s;
    int i;
    in >> i;
    getline(in, s);
    return i;
}

bool read_customer(ifstream &in, User &customer) {
    if (getline(in, customer.username)) {
        int gender;
        string s;
        
        getline(in, customer.password);
        getline(in, customer.name);
        
        customer.gender = Gender(read_int(in));
        
        getline(in, customer.phone_number);
        
        return true;
    }
    
    return false;
}

void add_customer(const User &user) {
    ofstream out;
    
    out.open(FILES.CUSTOMER, ios_base::app);
    
    out << user.username << endl;
    out << user.password << endl;
    out << user.name << endl;
    out << user.gender << endl;
    out << user.phone_number << endl;
    
    out.close();
}

bool find_customer(const string &username, User &customer) {
    ifstream in;
    
    in.open(FILES.CUSTOMER);
    
    while (read_customer(in, customer))
        if (customer.username == username) 
            break;
    
    in.close();
    return customer.username == username;
}

bool read_booking(ifstream &in, string &id, Booking &booking) {
    if (getline(in, id)) {
        getline(in, booking.customer_username);
        
        booking.day = read_int(in);
        booking.hour = read_int(in);
        booking.expert = read_int(in);
        booking.service = read_int(in);
        booking.service_type = ServiceType(read_int(in));
        
        return true;
    }
    
    return false;
}

void add_booking(const string &id, const Cache &cache) {
    const User &customer = cache.user;
    const Booking &booking = cache.booking;
    
    ofstream out;
    
    out.open(FILES.BOOKING, ios_base::app);
    
    out << id << endl;
    out << booking.customer_username << endl;
    out << booking.day << endl;
    out << booking.hour << endl;
    out << booking.expert << endl;
    out << booking.service << endl;
    out << booking.service_type << endl;
    
    out.close();
}

bool find_appointment(const string &id, Booking &appointment) {
    ifstream in;
    string booking_id;
    
    in.open(FILES.BOOKING);
    
    while (read_booking(in, booking_id, appointment))
        if (booking_id == id)
            break;
    
    in.close();
    return booking_id == id;
}

void save_cache(const Cache &cache) {
    ofstream out;
    
    out.open(FILES.CACHE);
    
    out << cache.new_booking_id << endl;
    out << cache.customer_count << endl;
    
    for (int day = 0; day < DAY_COUNT; day++)
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
            for (int expert = 0; expert < EXPERT_COUNT; expert++)
                out << cache.schedule[day][hour][expert];
    
    out << endl;
    
    for (int expert = 0; expert < EXPERT_COUNT; expert++) {
        for (int day = 0; day < DAY_COUNT; day++)
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                out << cache.experts[expert].consultation_schedule[day][hour] << ':';
        
        out << endl;
        
        for (int day = 0; day < DAY_COUNT; day++)
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                out << cache.experts[expert].treatment_schedule[day][hour] << ':';
        
        out << endl;
    }
}

void read_cache(Cache &cache) {
    ifstream in;
    string s;
    
    in.open(FILES.CACHE);
    
    if (in.is_open()) {
        cache.new_booking_id = read_int(in);
        cache.customer_count = read_int(in);
        
        for (int day = 0; day < DAY_COUNT; day++)
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                for (int expert = 0; expert < EXPERT_COUNT; expert++) {
                    char c;
                    in >> c;
                    cache.schedule[day][hour][expert] = c == '1';
                }
        
        getline(in, s);
    
        for (int expert = 0; expert < EXPERT_COUNT; expert++) {
            for (int day = 0; day < DAY_COUNT; day++)
                for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                    getline(in, cache.experts[expert].consultation_schedule[day][hour], ':');
            
            getline(in, s);
            
            for (int day = 0; day < DAY_COUNT; day++)
                for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                    getline(in, cache.experts[expert].treatment_schedule[day][hour], ':');
            
            getline(in, s);
        }
        
        in.close();
    }
}

void print_calendar(const Cache &cache) {
    const int CELL_WIDTH = 6;
    
    center(CELL_WIDTH * 7, 8, "Sep 2024") << endl;
    
    for (const string &day : SHORT_DAYS_OF_A_WEEK)
        center(CELL_WIDTH - 1, 3, day) << ' ';
    
    cout << endl;
    
    for (int week = 0; week < 5; week++) {
        for (int day = 0; day < (week == 5 ? 2 : 7); day++) {
            cout << setw(2) << week * 7 + day + 1 << ' ';
            cout << (check_availability(cache, CONSULTATION, day, -1, cache.booking.expert) ? 'C' : '-');
            cout << (check_availability(cache, TREATMENT, day, -1, cache.booking.expert) ? 'T' : '-');
            cout << ' ';
        }
        
        cout << endl;
    }
    
    cout << endl;
    cout << "C means consultation is available to book" << endl;
    cout << "T means treatment is available to book" << endl;
}

void print_schedule(const Cache &cache) {
    const int CELL_SIZE = 5;
    
    cout << "Schedule for " << (cache.booking.expert == -1 ? "all experts" : ("expert " + EXPERTS[cache.booking.expert].name)) << " on " << format_date(cache.booking.day) << endl << endl;
    
    if (cache.booking.expert == -1) {
        const string LABEL = "Experts";
        const int LONGEST_NAME_LENGTH = find_longest_expert_name_length();
        const int COLUMN_SIZE = (LONGEST_NAME_LENGTH > LABEL.length()) ? LONGEST_NAME_LENGTH : LABEL.length();
        
        cout << setfill(' ') << setw(COLUMN_SIZE) << left << LABEL;
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
            cout << '|' << 12 + hour << ":00";
        
        for (int expert = 0; expert < EXPERT_COUNT; expert++) {
            cout << endl;
            coutfill('-', COLUMN_SIZE);
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                cout << '+';
                
                coutfill('-', CELL_SIZE);
            }
            
            cout << endl << setfill(' ') << setw(COLUMN_SIZE) << left << EXPERTS[expert].name;
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                cout << '|';
                
                center(CELL_SIZE, 1, cache.schedule[cache.booking.day][hour][expert] ? "✗" : "✓");
            }
        }
    }
    else {
        string label = "Expert " + EXPERTS[cache.booking.expert].name;
        
        coutfill(' ', label.length());
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
            cout << '|' << 12 + hour << ":00";
        
        cout << endl;
        
        coutfill('-', label.length());
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
            cout << '+';
            
            coutfill('-', CELL_SIZE);
        }
        
        cout << endl << label;
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
            cout << '|';
            
            center(CELL_SIZE, 1, cache.schedule[cache.booking.day][hour][cache.booking.expert] ? "✗" : "✓");
        }
    }
    
    cout << endl;
}

int input_options(const Option options[], int option_count, bool &validation) {
    cout << endl;
    
    for (int i = 0; i < option_count; i++)
        cout << i + 1 << ": " << options[i].description << endl;
    
    if (!validation) {
        cout << endl << "Please enter valid option!" << endl;
    }
    
    int response;
    
    cout << endl << "Enter option: ";
    
    if (cin >> response) {
        if (response > 0 && response <= option_count) {
            validation = true;
            return response - 1;
        }
    }
    else {
        cin.clear();
        cin.ignore();
    }
    
    validation = false;
    
    return 0;
}

int ui(int state, bool &validation, Cache &cache) {
    const Option *options = nullptr;
    int option_count = 0;
    
    clear();
    cout << "COMPANY LOGO" << endl;
    line();
    
    if (!cache.user.username.empty()) {
        cout << "Currently login as " << cache.user.username << " (" << cache.user.name << ")" << endl;
        line();
    }
    
    switch (state) {
        case MAIN_MENU:
            static const Option MAIN_MENU_OPTIONS[] = {
                {"About us", ABOUT_US},
                {"I'm a customer", CUSTOMER_LOGIN},
                {"I'm a staff", STAFF_LOGIN},
                {"Exit", EXIT},
            };
            options = MAIN_MENU_OPTIONS;
            option_count = sizeof(MAIN_MENU_OPTIONS) / sizeof(Option);
            
            cout << "Main Menu" << endl;
            break;
        
        
        case ABOUT_US:
			static const Option ABOUT_US_OPTIONS[] = {{"Return to main menu", MAIN_MENU}};
			options = ABOUT_US_OPTIONS;
			option_count = sizeof(ABOUT_US_OPTIONS) / sizeof(Option);
            
            cout << "About Us" << endl;
            break;
        
        
        case CUSTOMER_LOGIN:
			static const Option CUSTOMER_LOGIN_OPTIONS[] = {{"Retry", CUSTOMER_LOGIN}, {"Return to main menu", MAIN_MENU}};
			options = CUSTOMER_LOGIN_OPTIONS;
			option_count = sizeof(CUSTOMER_LOGIN_OPTIONS) / sizeof(Option);
        {
            string username;
            
            cout << "Login or Register" << endl;
            
            if (validation) {
                cout << endl << "Username: ";
                
                cin >> username;
                
                if (!validate_username(username))
                    cout << endl << "Invalid username!" << endl;
                else if (find_customer(username, cache.user)) {
                    string password;
                    
                    cout << "Password: ";
                    
                    cin >> password;
                    
                    if (password == cache.user.password)
                        return CUSTOMER_MENU;
                    
                    cout << endl << "Incorrect password!" << endl;
                    cache.user = {}; // clear out cache for security purpose
                    break;
                }
                else {
                    string response;
                    
                    cache.user = {};
                    
                    cout << endl << "This username does not exist. Do you wish to register a new account with this username? (Y/n) ";
                    cin >> response;
                    
                    if (response == "n" || response == "N")
                        return MAIN_MENU;
                    else if (response != "y" && response != "Y")
                        break;
                    
                    cout << endl << "Enter your name: ";
                    cin >> cache.user.name;
                    cout << "Enter your gender (M/F): ";
                    cin >> response;
                    
                    if (response == "m" || response == "M")
                        cache.user.gender = MALE;
                    else if (response == "f" || response == "F")
                        cache.user.gender = FEMALE;
                    else {
                        cache.user = {};
                        cout << endl << "Invalid input!" << endl;
                        break;
                    }
                    
                    cout << "Enter your phone number (numbers only): ";
                    cin >> cache.user.phone_number;
                    
                    if (cache.user.phone_number.length() > 11 || cache.user.phone_number.find_first_not_of("0123456789-+") != cache.user.phone_number.npos) {
                        cache.user = {};
                        cout << endl << "Invalid input!" << endl;
                        break;
                    }
                    
                    cout << "Enter your password: ";
                    cin >> cache.user.password;
                    
                    cache.user.username = username;
                    
                    add_customer(cache.user);
                    
                    return CUSTOMER_MENU;
                }
            }
            
            break;
        }
            
            
        case CUSTOMER_MENU:
			static const Option CUSTOMER_MENU_OPTIONS[] = {
                {"View a list of available services", SERVICE_LIST},
                {"View a list of available experts", EXPERT_LIST},
                {"Book service", BOOKING},
                {"View appointments", APPOINTMENTS},
                {"Log out", LOGOUT},
            };
			options = CUSTOMER_MENU_OPTIONS;
			option_count = sizeof(CUSTOMER_MENU_OPTIONS) / sizeof(Option);
            
            cout << "Customer services" << endl;
            line();
            break;
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            line();
            
            cache.booking.service = -1;
            
            {
				Option options[SERVICE_COUNT + 1] = {};
				
				for (const Service &service : SERVICES)
					options[option_count++].description = service.name;
				
                options[option_count].description = "Return to menu";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return CUSTOMER_MENU;
				
                cache.booking.service = response;
                return VIEW_SERVICE;
			}
            
            
        case VIEW_SERVICE:
			static const Option VIEW_SERVICE_OPTIONS[] = {
                {"Book now", BOOKING},
                {"Go back to service list", SERVICE_LIST},
            };
			options = VIEW_SERVICE_OPTIONS;
			option_count = sizeof(VIEW_SERVICE_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            
            cout << service.name << " Service (Consultation: RM " << setprecision(2) << fixed << service.consultation_fee << "; Treatment: RM " << service.treatment_fee << ")" << endl;
            line();
            cout << service.description << endl;
            
            break;
        }
            
            
        case EXPERT_LIST:
            cout << "Our experts:" << endl;
            
            for (const User &expert : EXPERTS) {
                cout << "- " << expert.name << " (" << (expert.gender == MALE ? "Male" : "Female") << ") 📞 " << expert.phone_number << endl;
            }
            
            {
				Option options[EXPERT_COUNT + 1] = {};
                option_count = EXPERT_COUNT;
				
				for (size_t i = 0; i < option_count; i++)
                    options[i].description = "Book with " + EXPERTS[i].name;
				
                options[option_count].description = "Return to menu";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return CUSTOMER_MENU;
				
                validation = check_availability(cache, CONSULTATION, -1, -1, response);
                
                if (!validation)
                    return state;
                
                cache.booking.expert = response;
                return BOOKING;
			}
            
            
        case BOOKING:
			static const Option BOOKING_OPTIONS[] = {
                {"Book a day", BOOK_SELECT_DAY},
                {"Cancel", CUSTOMER_MENU},
            };
			options = BOOKING_OPTIONS;
			option_count = sizeof(BOOKING_OPTIONS) / sizeof(Option);
            
            cout << "Booking an appointment";
            
            if (cache.booking.expert != -1)
                cout << " with expert " << EXPERTS[cache.booking.expert].name;
            
            if (cache.booking.service != -1)
                cout << " of service " << SERVICES[cache.booking.service].name;
            
            cout << endl << endl;
            
            print_calendar(cache);
            break;
            
            
        case BOOK_SELECT_DAY:
        {
            string input;
            
            cout << "Booking an appointment";
            
            if (cache.booking.expert != -1)
                cout << " with expert " << EXPERTS[cache.booking.expert].name;
            
            if (cache.booking.service != -1)
                cout << " of service " << SERVICES[cache.booking.service].name;
            
            cout << endl << endl;
            
            print_calendar(cache);
            
            if (!validation)
                cout << endl << "Please input a valid day!" << endl;
            
            cout << endl << "Enter the day to book (x to cancel): ";
            
            cin >> input;
            
            if (input == "x")
                return BOOK_CANCEL;
            
            int day = atoi(input.c_str());
            
            if (day > 0 && --day < DAY_COUNT) {
                const bool consultation = check_availability(cache, CONSULTATION, day, -1, cache.booking.expert);
                const bool treatment = check_availability(cache, TREATMENT, day, -1, cache.booking.expert);
                
                if (consultation && treatment)
                    return BOOK_SELECT_SERVICE_TYPE;
                else if (consultation)
                    return BOOK_CONSULTATION;
                else if (treatment)
                    return BOOK_TREATMENT;
            }
            
            validation = false;
            return state;
        }
        
        
        case BOOK_SELECT_SERVICE_TYPE:
			static const Option BOOK_SELECT_SERVICE_TYPE_OPTIONS[] = {
                {"Book treatment", BOOK_TREATMENT},
                {"Book consultation", BOOK_CONSULTATION},
                {"Cancel", BOOK_CANCEL},
            };
			options = BOOK_SELECT_SERVICE_TYPE_OPTIONS;
			option_count = sizeof(BOOK_SELECT_SERVICE_TYPE_OPTIONS) / sizeof(Option);
            
            cout << "Booking an appointment";
            
            if (cache.booking.expert != -1)
                cout << " with expert " << EXPERTS[cache.booking.expert].name;
            
            if (cache.booking.service != -1)
                cout << " of service " << SERVICES[cache.booking.service].name;
            
            cout << "on " << format_date(cache.booking.day) << endl;
            break;
            
            
        case BOOK_TREATMENT:
            print_schedule(cache);
            
            cout << endl << "Select a time to book" << endl;
            
            {
				Option options[WORK_HOUR_COUNT] = {};
                option_count = WORK_HOUR_COUNT - 1;
				
				for (int i = 0; i < option_count; i++)
					options[i].description = to_string(OPENING_HOUR + i) + ":00~" + to_string(OPENING_HOUR + i + 2) + ":00";
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return BOOK_CANCEL;
                
                validation = check_availability(cache, TREATMENT, cache.booking.day, response, -1);
                
                if (!validation)
                    return state;
                
                cache.booking.service_type = TREATMENT;
                cache.booking.hour = response;
                return BOOK_SELECT_EXPERT;
			}
        
        
        case BOOK_CONSULTATION:
            print_schedule(cache);
            
            cout << endl << "Select a time to book + 1" << endl;
            
            {
				Option options[WORK_HOUR_COUNT + 1] = {};
                option_count = WORK_HOUR_COUNT;
				
				for (int i = 0; i < option_count; i++)
                    options[i].description = to_string(OPENING_HOUR + i) + ":00~" + to_string(OPENING_HOUR + i + 1) + ":00";
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return BOOK_CANCEL;
                
                validation = check_availability(cache, CONSULTATION, cache.booking.day, response, -1);
                
                if (!validation)
                    return state;
				
                cache.booking.service_type = CONSULTATION;
                cache.booking.hour = response;
                return BOOK_SELECT_EXPERT;
			}
        
        
        case BOOK_SELECT_EXPERT:
            cout << "Booking a " << (cache.booking.service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_booking_time(cache.booking) << endl;
            cout << endl << "Select an expert to book" << endl;
            
            {
                
				Option options[EXPERT_COUNT + 1] = {};
                option_count = EXPERT_COUNT;
				
				for (int i = 0; i < option_count; i++)
					options[i].description = "Expert " + EXPERTS[i].name;
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return BOOK_CANCEL;
				
                validation = check_availability(cache, cache.booking.service_type, cache.booking.day, cache.booking.hour, response);
                
                if (!validation)
                    return state;
                
                cache.booking.expert = response;
                return BOOK_SELECT_SERVICE;
			}
        
        
        case BOOK_SELECT_SERVICE:
            if (cache.booking.service != -1)
                return BOOK_CONFIRM;
            
            cout << "Booking a " << (cache.booking.service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_booking_time(cache.booking) << endl;
            
            {
				Option options[SERVICE_COUNT + 1] = {};
				
				for (const Service &service: SERVICES)
					options[option_count++].description = "Service " + service.name;
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return BOOK_CANCEL;
				
                cache.booking.service = response;
                return BOOK_CONFIRM;
			}
        
            
        case BOOK_CONFIRM:
			static const Option BOOK_CONFIRM_OPTIONS[] = {
                {"Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
                {"No, there are mistakes in the booking information", BOOK_CANCEL},
            };
			options = BOOK_CONFIRM_OPTIONS;
			option_count = sizeof(BOOK_CONFIRM_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            const User &customer = cache.user;
            
            cout << "Please confirm that the following information are correct" << endl;
            line();
            cout << "Name: " << customer.name << endl;
            cout << "Gender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
            cout << "Contact number: " << customer.phone_number << endl << endl;
            cout << (cache.booking.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_booking_time(cache.booking) << endl;
            cout << "Expert: " << EXPERTS[cache.booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            cout << endl << "Fee: RM " << setprecision(2) << fixed << (cache.booking.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee) << endl;
            break;
        }
        
        
        case BOOK_PAYMENT:
			static const Option BOOK_PAYMENT_OPTIONS[] = {
                {"Credit card", BOOK_SUCCESS},
                {"EWallet", BOOK_SUCCESS},
                {"Online banking", BOOK_SUCCESS},
            };
			options = BOOK_PAYMENT_OPTIONS;
			option_count = sizeof(BOOK_PAYMENT_OPTIONS) / sizeof(Option);
            
            cout << "Select your payment option" << endl;
            break;
        
        
        case BOOK_SUCCESS:
			static const Option BOOK_SUCCESS_OPTIONS[] = {{"Return to main menu", CUSTOMER_MENU}};
			options = BOOK_SUCCESS_OPTIONS;
			option_count = sizeof(BOOK_SUCCESS_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            
            string id = "B" + to_string(cache.new_booking_id);
            
            Booking &booking = cache.booking;
            ExpertSchedule &schedule = cache.experts[cache.booking.expert];
            
            (booking.service_type == TREATMENT ? schedule.treatment_schedule : schedule.consultation_schedule)[booking.day][booking.hour] = id;
            
            booking.customer_username = cache.user.username;
            
            add_booking(std::move(id), cache);
            
            for (int i = 0; i < (booking.service_type == TREATMENT ? 2 : 1); i++)
                cache.schedule[cache.booking.day][cache.booking.hour + i][cache.booking.expert] = true;
            
            cout << "Successfully booked the appointment!" << endl;
            cout << endl << "Appointment id: B" << cache.new_booking_id++ << endl;
            cout << (booking.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_booking_time(cache.booking) << endl;
            cout << "Expert: " << EXPERTS[booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            
            booking.hour = -1;
            booking.service = -1;
            booking.expert = -1;
            
            break;
        }
        
        
        case BOOK_CANCEL:
            cache.booking.day = -1;
            cache.booking.hour = -1;
            cache.booking.service = -1;
            cache.booking.expert = -1;
            return BOOKING;
        
        
        case APPOINTMENTS:
			static const Option APPOINTMENTS_OPTIONS[] = {
                {"View another appointment", APPOINTMENTS},
                {"Go back", CUSTOMER_MENU},
            };
			options = APPOINTMENTS_OPTIONS;
			option_count = sizeof(APPOINTMENTS_OPTIONS) / sizeof(Option);
        {
            string id;
            Booking appointment;
            
            cout << "Please enter the appointment id: ";
            cin >> id;
            cout << endl;
            
            if (find_appointment(id, appointment)) {
                cout << "Your appointment is a " << SERVICES[appointment.service].name << " " << (appointment.service_type == TREATMENT ? "treatment" : "consultation") << " during " << format_booking_time(appointment) << endl;
                cout << "Expert in charge: " << EXPERTS[appointment.expert].name << endl;
                cout << "Service description: " << SERVICES[appointment.service].description << endl;
            }
            else {
                cout << "Id not found!" << endl;
            }
            
            break;
        }
        
            
        case STAFF_LOGIN:
			static const Option STAFF_LOGIN_OPTIONS[] = {
                {"Retry", STAFF_LOGIN},
                {"Return to main menu", MAIN_MENU},
            };
			options = STAFF_LOGIN_OPTIONS;
			option_count = sizeof(STAFF_LOGIN_OPTIONS) / sizeof(Option);
        {
            string username, password;
            
            cout << "Login as staff" << endl << endl;
            cout << "Username: ";
            cin >> username;
            
            if (!validate_username(username)) {
                cout << "Invalid username!" << endl;
                break;
            }
            if (username == ADMIN.username)
                cache.user = ADMIN;
            else {
                bool username_found = false;
                
                for (const User &expert : EXPERTS) {
                    if (username == expert.username) {
                        cache.user = expert;
                        username_found = true;
                        break;
                    }
                }
                
                if (!username_found) {
                    cout << endl << "Username not found!" << endl;
                    break;
                }
            }
            
            cout << "Password: ";
            cin >> password;
            
            if (password == cache.user.password)
                return STAFF_MENU;
            else
                cout << endl << "Incorrect password!" << endl;
            
            cache.user = {};
            break;
        }
            
            
        case STAFF_MENU:
			static const Option STAFF_MENU_OPTIONS[] = {
                {"View treatment schedule", TREATMENT_SCHEDULE},
                {"View consultation schedule", CONSULTATION_SCHEDULE},
                {"View customers", CUSTOMER_DETAILS},
                {"View sales report", SALES_REPORT},
                {"Log out", LOGOUT},
            };
			options = STAFF_MENU_OPTIONS;
			option_count = sizeof(STAFF_MENU_OPTIONS) / sizeof(Option);
            
            cout << "Staff menu" << endl;
            break;
            
            
        case TREATMENT_SCHEDULE:
            cache.booking.service_type = TREATMENT;
            cache.schedule_view_week = 0;
            return SCHEDULE_VIEW;
            
            
        case CONSULTATION_SCHEDULE:
            cache.booking.service_type = CONSULTATION;
            cache.schedule_view_week = 0;
            return SCHEDULE_VIEW;
            
        
        case SCHEDULE_VIEW:
			static const Option SCHEDULE_VIEW_OPTIONS[] = {
                {"Previous week", SCHEDULE_VIEW_PREV_WEEK},
                {"Next week", SCHEDULE_VIEW_NEXT_WEEK},
                {"Return to main menu", STAFF_MENU},
            };
			options = SCHEDULE_VIEW_OPTIONS;
			option_count = sizeof(SCHEDULE_VIEW_OPTIONS) / sizeof(Option);
        {
            if (cache.user.username == ADMIN.username)
                return INVALID_USER;
            
            int index = 0;
            
            for (const User &expert : EXPERTS)
                if (expert.username == cache.user.username)
                    break;
                else
                    index++;
            
            int day_offset = cache.schedule_view_week * 7;
            int day_count = cache.schedule_view_week == 4 ? 2 : 7;
            
            const ExpertSchedule &schedule = cache.experts[index];
            
            const int LABEL_SIZE = 5;
            const int CELL_SIZE = 5;
            const int LINE_WIDTH = LABEL_SIZE + 7 * (CELL_SIZE + 1);
            
            string s = format_date(cache.schedule_view_week * 7) + " ~ " + format_date(cache.schedule_view_week * 7 + day_count - 1);
            
            center(LINE_WIDTH, s.length(), s) << endl;
            
            coutfill(' ', 5);
            
            for (int day = 0; day < day_count; day++) {
                cout << '|';
                center(5, 3, SHORT_DAYS_OF_A_WEEK[day]);
            }
            
            cout << endl;
            
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                coutfill('-', LABEL_SIZE);
                
                for (int j = 0; j < day_count; j++) {
                    cout << '+';
                    coutfill('-', CELL_SIZE);
                }
                
                cout << endl << OPENING_HOUR + hour << ":00";
                
                for (int day = 0; day < day_count; day++) {
                    const string *id = nullptr;
                    const string (&day_schedule)[WORK_HOUR_COUNT] = (cache.booking.service_type == TREATMENT ? schedule.treatment_schedule : schedule.consultation_schedule)[day_offset + day];
                    
                    if (!day_schedule[hour].empty())
                        id = &day_schedule[hour];
                    
                    if (cache.booking.service_type == TREATMENT && hour > 0 && !day_schedule[hour - 1].empty())
                        id = &day_schedule[hour - 1];
                    
                    cout << '|' << (id == nullptr ? "  -  " : *id);
                }
                
                cout << endl;
            }
            
            break;
        }
        
        
        case SCHEDULE_VIEW_PREV_WEEK:
            if (cache.schedule_view_week > 0)
                cache.schedule_view_week--;
            
            return SCHEDULE_VIEW;
        
        
        case SCHEDULE_VIEW_NEXT_WEEK:
            if (cache.schedule_view_week < 4)
                cache.schedule_view_week++;
            
            return SCHEDULE_VIEW;
        
            
        case CUSTOMER_DETAILS:
            cache.customer_view_page = 0;
            return CUSTOMER_VIEW;
        
        
        case CUSTOMER_VIEW:
			static const Option CUSTOMER_VIEW_OPTIONS[] = {
                {"Previous page", CUSTOMER_VIEW_PREV_PAGE},
                {"Next page", CUSTOMER_VIEW_NEXT_PAGE},
                {"Return to main menu", STAFF_MENU},
            };
			options = CUSTOMER_VIEW_OPTIONS;
			option_count = sizeof(CUSTOMER_VIEW_OPTIONS) / sizeof(Option);
            
            if (cache.customer_count > 0) {
                cout << "Customers (Page " << cache.customer_view_page + 1 << " of " << (cache.customer_count + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE << ')' << endl;
                line();
                
                int offset = cache.customer_view_page * CUSTOMER_ENTRIES_PER_PAGE;
                
                ifstream in;
                User customer;
                
                in.open(FILES.CUSTOMER);
                
                for (int i = 0; i < offset; i++)
                    read_customer(in, customer);
                
                for (int i = 0; i < CUSTOMER_ENTRIES_PER_PAGE && read_customer(in, customer); i++)
                    cout << offset + i + 1 << ". " << customer.name << " (" << (customer.gender == MALE ? "Male" : "Female") << ") 📞 " << customer.phone_number << endl;
                
                in.close();
            }
            else
                cout << "Customer list is empty" << endl;
            
            break;
        
        
        case CUSTOMER_VIEW_PREV_PAGE:
            if (cache.customer_view_page > 0)
                cache.customer_view_page--;
            
            return CUSTOMER_VIEW;
        
        
        case CUSTOMER_VIEW_NEXT_PAGE:
            if (cache.customer_view_page + 1 < (cache.customer_count + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE)
                cache.customer_view_page++;
            
            return CUSTOMER_VIEW;
            
            
        case SALES_REPORT:
			static const Option SALES_REPORT_OPTIONS[] = {
                {"Customer summary", SALES_REPORT_CUSTOMER_SUMMARY},
                {"Expert summary", SALES_REPORT_EXPERT_SUMMARY},
                {"Finance summary", SALES_REPORT_FINANCE_SUMMARY},
            };
			options = SALES_REPORT_OPTIONS;
			option_count = sizeof(SALES_REPORT_OPTIONS) / sizeof(Option);
            
            cout << "Sales report" << endl;
            break;
        
        
        case SALES_REPORT_CUSTOMER_SUMMARY:
			static const Option SALES_REPORT_CUSTOMER_SUMMARY_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
			options = SALES_REPORT_CUSTOMER_SUMMARY_OPTIONS;
			option_count = sizeof(SALES_REPORT_CUSTOMER_SUMMARY_OPTIONS) / sizeof(Option);
            
            cout << "Sales Report: Customer Summary" << endl;
            line();
            
            if (cache.customer_count > 0) {
                // todo
                
                // const User *most_appointments = nullptr;
                // const User *most_paid = nullptr;
                
                // for (const auto &p : cache.customer_database) {
                //     const auto &customer = p.second;
                //     const auto &data = *customer.customer_data;
                    
                //     if (most_appointments == nullptr || most_appointments->customer_data->booking_history.size() < data.booking_history.size())
                //         most_appointments = &customer;
                    
                //     if (most_paid == nullptr || most_paid->customer_data->total_payment < data.total_payment)
                //         most_paid = &customer;
                // }
                
                // cout << "Customer who booked the highest count of appointments: " << most_appointments->name << endl;
                // print_customer_data(*most_appointments);
                
                // line();
                
                // cout << "Customer who paid most for our services: " << most_paid->name << endl;
                // print_customer_data(*most_paid);
            }
            else
                cout << "Not enough data" << endl;
            
            break;
            
        
        
        case SALES_REPORT_EXPERT_SUMMARY:
			static const Option SALES_REPORT_EXPERT_SUMMARY_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
			options = SALES_REPORT_EXPERT_SUMMARY_OPTIONS;
			option_count = sizeof(SALES_REPORT_EXPERT_SUMMARY_OPTIONS) / sizeof(Option);
        {
            // todo
            
            // constexpr ConstStr HOURS_WORKED = "Hours Worked", TOTAL_SALES = "Total Sales";
            // constexpr auto MAX_RECORD_NAME_LEN = max(HOURS_WORKED.len, TOTAL_SALES.len);
            
            // coutfill<' ', MAX_RECORD_NAME_LEN>();
            
            break;
        }
        
        
        case SALES_REPORT_FINANCE_SUMMARY:
			static const Option SALES_REPORT_FINANCE_SUMMARY_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
			options = SALES_REPORT_FINANCE_SUMMARY_OPTIONS;
			option_count = sizeof(SALES_REPORT_FINANCE_SUMMARY_OPTIONS) / sizeof(Option);
            break;
        
        
        case INVALID_USER:
			static const Option INVALID_USER_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
			options = INVALID_USER_OPTIONS;
			option_count = sizeof(INVALID_USER_OPTIONS) / sizeof(Option);
            cout << "Unavailable to this user" << endl;
            break;
            
            
        case LOGOUT:
			static const Option LOGOUT_OPTIONS[] = {{"Return to main menu", MAIN_MENU}};
			options = LOGOUT_OPTIONS;
			option_count = sizeof(LOGOUT_OPTIONS) / sizeof(Option);
            cache.user = {};
            cout << "Successfully logged out!" << endl;
            break;
        
        
        case EXIT:
            cout << "Thank you for using our service" << endl;
            return EXIT;
        
        
        default:
            break;
    }
    
    int response = input_options(options, option_count, validation);
    
    return validation ? options[response].link : state;
}

int main() {
    int state = MAIN_MENU;
    Cache cache {};
    bool validation = true;
    
    cache.booking.day = -1;
    cache.booking.hour = -1;
    cache.booking.service = -1;
    cache.booking.expert = -1;
    cache.new_booking_id = 1000;
    
    read_cache(cache);
    
    while ((state = ui(state, validation, cache)) != EXIT);
    
    ui(EXIT, validation, cache);
    
    save_cache(cache);
    
    return 0;
}
