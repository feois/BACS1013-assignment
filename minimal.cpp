#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

// Clear console
void clear() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    system("cls");
#else
    cout << "\E[H\E[2J";
#endif
}

// Constants used throughout the program
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

// Enumeration for the different states of screens in the program
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
    BOOK_SELECT_SERVICE_TYPE,
    BOOK_TREATMENT,
    BOOK_CONSULTATION,
    BOOK_SELECT_EXPERT,
    BOOK_SELECT_SERVICE,
    BOOK_CONFIRM,
    BOOK_PAYMENT,
    BOOK_SUCCESS,
    
    
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
    
    
    LOGOUT,
    
    EXIT,
};

// Structure to store booking details
struct Service {
    string name;
    string description;
    double consultation_fee;
    double treatment_fee;
};

// Structure to store user and booking cache information
struct User {
    string name;
    Gender gender;
    string phone_number;
    string username;
    string password;
};

// Structure to store booking ids in the expert's schedule
struct ExpertSchedule {
    string treatment_schedule[DAY_COUNT][WORK_HOUR_COUNT];
    string consultation_schedule[DAY_COUNT][WORK_HOUR_COUNT];
};

// Structure to store booking information
struct Booking {
    ServiceType service_type;
    int service;
    int expert;
    int day;
    int hour;
    string customer_username;
};

// Structure to store cache data passed around in the program
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

// Structure to store an option and its link to other page
struct Option {
    string description;
    State link;
};

// File paths
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
    {"Skin Care", "Helps identify your skin diseases to cure them, help protect or enhance the skin's looks depending on the state of your skin.", 125.00, 225.00},
    {"Face Makeup", "Enables you to look good and hides your acnes before an event, to help you look the best possible you can be.", 200.00, 450.00},
    {"Hair Coloring", "Turn your hair into any color you want. With our long lasting dye, you can express yourself for a long time.", 250.00, 500.00},
};

const User EXPERTS[] = {
    { "Ang Zi En", FEMALE, "+60 11-1083 7183", "aze", "12345678" },
    { "Wai Chee Han", MALE, "+60 16-899 2283", "waich", "wch123" },
    { "Wilson Ang Shao En", MALE, "+60 11-7243 3165", "wase", "051224021039" },
};

const User ADMIN = { "Administrator", MALE, "-", "admin", "12345678" };

// Prints c for n times
void fill_cout(char c, int n) {
    if (n > 0)
        cout << setfill(c) << setw(n) << c;
}

// Prints a line
void print_line() {
    fill_cout('-', 40);
    cout << endl;
}

// Centers s in a string with length target_length
ostream &center(int target_length, const string &s) {
    if (s.length() > target_length)
        cout << s;
    else {
        int padding = target_length - s.length();
        int left = padding / 2;
        int right = padding - left;
        
        fill_cout(' ', left);
        
        cout << s;
        
        fill_cout(' ', right);
    }
    
    return cout;
}

// Finds longest expert name length
int find_longest_expert_name_length() {
    int length = 0;
    
    for (const User &expert : EXPERTS)
        if (expert.name.length() > length)
            length = expert.name.length();
    
    return length;
}

// Formats a date string
string format_date(int day) {
    return DAYS_OF_A_WEEK[day % 7] + " " + to_string(day + 1) + " September 2024";
}

// Formats a date and time string
string format_time(int day, int hour) {
    return format_date(day) + " " + to_string(hour + OPENING_HOUR) + ":00";
}

// Formats a date, starting time and end time string
string format_booking_time(const Booking &booking) {
    return format_time(booking.day, booking.hour) + "~" + to_string(booking.hour + OPENING_HOUR + (booking.service_type == TREATMENT ? 2 : 1)) + ":00";
}

// Validates username string
bool validate_username(const string &s) {
    for (char c : s)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
            return false;
    
    return true;
}

// Checks if a combination of appointment's conditions is available, -1 will check all possible inputs
bool check_availability(const Cache &cache, ServiceType type, int day, int hour, int expert) {
    if (day == -1) { // Checks every day
        for (int i = 0; i < DAY_COUNT; i++)
            if (check_availability(cache, type, i, hour, expert))
                return true;
        
        return false;
    }
    
    if (hour == -1) { // Checks every hour
        for (int i = 0; i < WORK_HOUR_COUNT - (type == TREATMENT ? 1 : 0); i++)
            if (check_availability(cache, type, day, i, expert))
                return true;
        
        return false;
    }
    
    if (expert == -1) { // Checks every expert
        for (int i = 0; i < EXPERT_COUNT; i++)
            if (check_availability(cache, type, day, hour, i))
                return true;
        
        return false;
    }
    
    switch (type) {
        case TREATMENT: // Checks if there's two hour free time
            return !(cache.schedule[day][hour][expert] || cache.schedule[day][hour + 1][expert]);
        
        case CONSULTATION: // Checks if there's one hour free time
            return !cache.schedule[day][hour][expert];
    }
}

// Reads an integer from file stream
int read_int(ifstream &in) {
    string s;
    int i;
    in >> i;
    getline(in, s); // Skip a line
    return i;
}

// Reads customer data from file stream if possible
bool read_customer(ifstream &in, User &customer) {
    if (getline(in, customer.username)) { // if successfully read username
        getline(in, customer.password);
        getline(in, customer.name);
        
        customer.gender = Gender(read_int(in));
        
        getline(in, customer.phone_number);
        
        return true;
    }
    
    return false;
}

// Appends new customer data to file
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

// Finds customer data with matching username
bool find_customer(const string &username, User &customer) {
    ifstream in;
    
    in.open(FILES.CUSTOMER);
    
    while (read_customer(in, customer)) // loop through all customers
        if (customer.username == username) 
            break;
    
    in.close();
    return customer.username == username;
}

// Reads booking information from file stream if possible
bool read_booking(ifstream &in, string &id, Booking &booking) {
    if (getline(in, id)) { // if successfully reads id
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

// Appends new booking information into file
void add_booking(const string &id, const Booking &booking) {
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

// Finds appointment information with matching id
bool find_appointment(const string &id, Booking &appointment) {
    ifstream in;
    string booking_id;
    
    in.open(FILES.BOOKING);
    
    while (read_booking(in, booking_id, appointment)) // loops through all bookings
        if (booking_id == id)
            break;
    
    in.close();
    return booking_id == id;
}

// Saves cache data into file
void save_cache(const Cache &cache) {
    ofstream out;
    
    out.open(FILES.CACHE);
    
    out << cache.new_booking_id << endl;
    out << cache.customer_count << endl;
    
    for (int day = 0; day < DAY_COUNT; day++)
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
            for (int expert = 0; expert < EXPERT_COUNT; expert++)
                out << cache.schedule[day][hour][expert]; // writes an hour time slot, 1 if occupied, 0 if free
    
    out << endl;
    
    for (int expert = 0; expert < EXPERT_COUNT; expert++) { // writes all booking ids stored in schedule
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

// Reads cache data from file if file exists
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
                    in >> c; // reads a character
                    cache.schedule[day][hour][expert] = c == '1'; // if 1 means occupied, 0 means free
                }
        
        getline(in, s); // Skip a line
    
        for (int expert = 0; expert < EXPERT_COUNT; expert++) {
            for (int day = 0; day < DAY_COUNT; day++)
                for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                    getline(in, cache.experts[expert].consultation_schedule[day][hour], ':'); // reads until :
            
            getline(in, s); // Skip a line
            
            for (int day = 0; day < DAY_COUNT; day++)
                for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
                    getline(in, cache.experts[expert].treatment_schedule[day][hour], ':'); // reads until :
            
            getline(in, s); // Skip a line
        }
        
        in.close();
    }
}

// Prints calendar
void print_calendar(const Cache &cache) {
    const int CELL_WIDTH = 6;
    
    center(CELL_WIDTH * 7, "Sep 2024") << endl;
    
    for (const string &day : SHORT_DAYS_OF_A_WEEK) // prints column labels
        center(CELL_WIDTH - 1, day) << ' ';
    
    cout << endl;
    
    for (int week = 0; week < 5; week++) {
        for (int day = 0; day < (week == 4 ? 2 : 7); day++) {
            cout << setw(2) << week * 7 + day + 1 << ' ';
            cout << (check_availability(cache, CONSULTATION, week * 7 + day, -1, cache.booking.expert) ? 'C' : '-');
            cout << (check_availability(cache, TREATMENT, week * 7 + day, -1, cache.booking.expert) ? 'T' : '-');
            cout << ' ';
        }
        
        cout << endl;
    }
    
    cout << endl;
    cout << "C means consultation is available to book" << endl;
    cout << "T means treatment is available to book" << endl;
}

// Prints booking status
void print_booking_status(const Booking &booking, bool is_service_type_selected) {
    cout << "Booking an appointment";
    
    if (booking.expert != -1)
        cout << " with expert " << EXPERTS[booking.expert].name;
    
    if (booking.service != -1) {
        cout << " of service " << SERVICES[booking.service].name;
        
        if (is_service_type_selected)
            cout << ' ' << (booking.service_type == TREATMENT ? "treatment" : "consultation");
    }
    
    if (booking.day != -1) {
        cout << " on " << (booking.hour != -1 ? format_booking_time(booking) : format_date(booking.day));
    }
    
    cout << endl;
}

// Prints expert(s)' schedule
void print_schedule(const Cache &cache) {
    const int CELL_SIZE = 5;
    
    print_booking_status(cache.booking, true);
    
    cout << endl << "Schedule for " << (cache.booking.expert == -1 ? "all experts" : ("expert " + EXPERTS[cache.booking.expert].name)) << " on " << format_date(cache.booking.day) << endl << endl;
    
    if (cache.booking.expert == -1) { // all experts
        const string LABEL = "Experts";
        const int LONGEST_NAME_LENGTH = find_longest_expert_name_length();
        const int COLUMN_SIZE = (LONGEST_NAME_LENGTH > LABEL.length()) ? LONGEST_NAME_LENGTH : LABEL.length(); // longest row label's length
        
        cout << setfill(' ') << setw(COLUMN_SIZE) << left << LABEL;
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) // prints column labels
            cout << '|' << 12 + hour << ":00";
        
        for (int expert = 0; expert < EXPERT_COUNT; expert++) {
            cout << endl;
            fill_cout('-', COLUMN_SIZE); // print table border
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                cout << '+'; // prints line intersection
                
                fill_cout('-', CELL_SIZE);
            } // finishes printing table border
            
            cout << endl << setfill(' ') << setw(COLUMN_SIZE) << left << EXPERTS[expert].name; // prints row label
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                cout << '|';
                
                center(CELL_SIZE, cache.schedule[cache.booking.day][hour][expert] ? "X" : "O");
            }
        }
    }
    else { // chosen single expert
        string label = "Expert " + EXPERTS[cache.booking.expert].name;
        
        fill_cout(' ', label.length());
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++)
            cout << '|' << 12 + hour << ":00";
        
        cout << endl;
        
        fill_cout('-', label.length()); // prints table border
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
            cout << '+';
            
            fill_cout('-', CELL_SIZE);
        } // finishes printing table border
        
        cout << endl << label;
        
        for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
            cout << '|';
            
            center(CELL_SIZE, cache.schedule[cache.booking.day][hour][cache.booking.expert] ? "X" : "O");
        }
    }
    
    cout << endl;
}

// Print available options and interprets response
int input_options(const Option options[], int option_count, bool &validation) {
    cout << endl;
    print_line();
    cout << endl << "Please select one of the following options:" << endl;
    
    for (int i = 0; i < option_count; i++)
        cout << i + 1 << ": " << options[i].description << endl;
    
    if (!validation)
        cout << endl << "Please enter a valid option!" << endl;
    
    int response;
    cout << endl << "Enter option: ";
    
    if (cin >> response) {
        if (response > 0 && response <= option_count) { // out of range
            validation = true;
            return response - 1;
        }
    }
    else { // invalid input (not integer)
        cin.clear();
        cin.ignore();
    }
    
    validation = false;
    
    return 0;
}

const Option MAIN_MENU_OPTIONS[] = {
    {"About us", ABOUT_US},
    {"I'm a customer", CUSTOMER_LOGIN},
    {"I'm a staff", STAFF_LOGIN},
    {"Exit", EXIT},
};
const Option ABOUT_US_OPTIONS[] = {{"Return to main menu", MAIN_MENU}};
const Option CUSTOMER_LOGIN_OPTIONS[] = {{"Retry", CUSTOMER_LOGIN}, {"Return to main menu", MAIN_MENU}};
const Option CUSTOMER_MENU_OPTIONS[] = {
    {"View a list of available services", SERVICE_LIST},
    {"View a list of available experts", EXPERT_LIST},
    {"Book service", BOOKING},
    {"View appointments", APPOINTMENTS},
    {"Log out", LOGOUT},
};
const Option VIEW_SERVICE_OPTIONS[] = {
    {"Book now", BOOKING},
    {"Go back to service list", SERVICE_LIST},
};
const Option BOOK_SELECT_SERVICE_TYPE_OPTIONS[] = {
    {"Book treatment (2 hour)", BOOK_TREATMENT},
    {"Book consultation (1 hour)", BOOK_CONSULTATION},
    {"Cancel", CUSTOMER_MENU},
};
const Option BOOK_CONFIRM_OPTIONS[] = {
    {"Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
    {"No, there are mistakes in the booking information", CUSTOMER_MENU},
};
const Option BOOK_PAYMENT_OPTIONS[] = {
    {"Credit card", BOOK_SUCCESS},
    {"EWallet", BOOK_SUCCESS},
    {"Online banking", BOOK_SUCCESS},
};
const Option BOOK_SUCCESS_OPTIONS[] = {{"Return to main menu", CUSTOMER_MENU}};
const Option APPOINTMENTS_OPTIONS[] = {
    {"View another appointment", APPOINTMENTS},
    {"Go back", CUSTOMER_MENU},
};
const Option STAFF_LOGIN_OPTIONS[] = {
    {"Retry", STAFF_LOGIN},
    {"Return to main menu", MAIN_MENU},
};
const Option STAFF_MENU_OPTIONS[] = {
    {"View treatment schedule", TREATMENT_SCHEDULE},
    {"View consultation schedule", CONSULTATION_SCHEDULE},
    {"View customers", CUSTOMER_DETAILS},
    {"View sales report", SALES_REPORT},
    {"Log out", LOGOUT},
};
const Option SCHEDULE_VIEW_OPTIONS[] = {
    {"Previous week", SCHEDULE_VIEW_PREV_WEEK},
    {"Next week", SCHEDULE_VIEW_NEXT_WEEK},
    {"Return to main menu", STAFF_MENU},
};
const Option CUSTOMER_VIEW_OPTIONS[] = {
    {"Previous page", CUSTOMER_VIEW_PREV_PAGE},
    {"Next page", CUSTOMER_VIEW_NEXT_PAGE},
    {"Return to main menu", STAFF_MENU},
};
const Option SALES_REPORT_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
const Option INVALID_USER_OPTIONS[] = {{"Return to main menu", STAFF_MENU}};
const Option LOGOUT_OPTIONS[] = {{"Return to main menu", MAIN_MENU}};

// Displays ui and process main logics
int ui(int state, bool &validation, Cache &cache) {
    const Option *options = nullptr;
    int option_count = 0;
    
    clear();
    
    cout << R"(        ,-----.                           ,--.           )" << endl;
    cout << R"(,--.,--.|  |) /_  ,---.  ,--,--.,--.,--.,-'  '-.,--. ,--.)" << endl;
    cout << R"(|  ||  ||  .-.  \| .-. :' ,-.  ||  ||  |'-.  .-' \  '  / )" << endl;
    cout << R"('  ''  '|  '--' /\   --.\ '-'  |'  ''  '  |  |    \   '  )" << endl;
    cout << R"(`------' `------' `----' `--`--' `----'   `--'  .-'  /   )" << endl;
    cout << R"(                                                `---'    )" << endl;
    
    if (!cache.user.username.empty()) {
        cout << "Currently login as " << cache.user.username << " (" << cache.user.name << ")" << endl;
        print_line();
    }
    
    switch (state) {
        case MAIN_MENU:
            options = MAIN_MENU_OPTIONS;
            option_count = sizeof(MAIN_MENU_OPTIONS) / sizeof(Option);
            
            cout << "Main Menu" << endl;
            break;
        
        
        case ABOUT_US:
			options = ABOUT_US_OPTIONS;
			option_count = sizeof(ABOUT_US_OPTIONS) / sizeof(Option);
            
            cout << "About Us" << endl << endl;
            
            cout << "At uBeauty, we prioritize our customer's needs, and we will help you reach your true beauty potential." << endl << endl;
            cout << "We offer services such as skin care, face makeup and hair coloring, which will help you achieve your best looks." << endl << endl;
            cout << "Whether it is for events, dates or just to look better, we will be here to assist you for you to be ready for anything." << endl;
            
            break;
        
        
        case CUSTOMER_LOGIN:
			options = CUSTOMER_LOGIN_OPTIONS;
			option_count = sizeof(CUSTOMER_LOGIN_OPTIONS) / sizeof(Option);
        {
            string username;
            
            cout << "Login or Register" << endl;
            
            if (validation) { // is not redirected from same page due to invalid input
                cout << endl << "Username: ";
                
                cin >> username;
                
                if (!validate_username(username))
                    cout << endl << "Invalid username!" << endl;
                else if (find_customer(username, cache.user)) { // username exists
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
                    char response;
                    
                    cache.user = {}; // clear out cache for security purpose
                    
                    cout << endl << "This username does not exist. Do you wish to register a new account with this username? (y/n) ";
                    cin >> response;
                    cin.ignore();
                    
                    if (response == 'n' || response == 'N')
                        return MAIN_MENU;
                    else if (response != 'y' && response != 'Y') // invalid input
                        break;
                    
                    cout << endl << "Enter your name (not username): ";
                    getline(cin, cache.user.name);
                    cout << "Enter your gender (M/F): ";
                    cin >> response;
                    
                    if (response == 'm' || response == 'M')
                        cache.user.gender = MALE;
                    else if (response == 'f' || response == 'F')
                        cache.user.gender = FEMALE;
                    else { // invalid input
                        cache.user = {}; // clear out cache for security purpose
                        cout << endl << "Invalid input!" << endl;
                        break;
                    }
                    
                    cout << "Enter your phone number (numbers only): ";
                    cin >> cache.user.phone_number;
                    
                    // validate phone number
                    if (cache.user.phone_number.length() > 11 || cache.user.phone_number.find_first_not_of("0123456789-+") != cache.user.phone_number.npos) {
                        cache.user = {}; // clear out cache for security purpose
                        cout << endl << "Invalid input!" << endl;
                        break;
                    }
                    
                    cout << "Enter your password: ";
                    cin >> cache.user.password;
                    
                    cache.user.username = username;
                    cache.customer_count++;
                    add_customer(cache.user);
                    save_cache(cache);
                    
                    return CUSTOMER_MENU;
                }
            }
            
            break;
        }
            
            
        case CUSTOMER_MENU:
			options = CUSTOMER_MENU_OPTIONS;
			option_count = sizeof(CUSTOMER_MENU_OPTIONS) / sizeof(Option);
            
            cache.booking.day = -1;
            cache.booking.hour = -1;
            cache.booking.expert = -1;
            cache.booking.service = -1;
            
            cout << "Customer services" << endl;
            break;
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            
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
			options = VIEW_SERVICE_OPTIONS;
			option_count = sizeof(VIEW_SERVICE_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            
            cout << service.name << " Service (Consultation: RM " << setprecision(2) << fixed << service.consultation_fee << "; Treatment: RM " << service.treatment_fee << ")" << endl;
            print_line();
            cout << service.description << endl;
            
            break;
        }
            
            
        case EXPERT_LIST:
            cout << "Our experts:" << endl;
            
            for (const User &expert : EXPERTS)
                cout << "- " << expert.name << " (" << (expert.gender == MALE ? "Male" : "Female") << ") Phone number: " << expert.phone_number << endl;
            
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
				
                cache.booking.expert = response;
                return BOOKING;
			}
            
            
        case BOOKING:
        {
            string input;
            
            print_booking_status(cache.booking, false);
            
            cout << endl;
            
            print_calendar(cache);
            
            if (!validation)
                cout << endl << "Please input a valid day!" << endl;
            
            validation = true;
            
            cout << endl << "Enter the day to book (x to cancel): ";
            
            cin >> input;
            
            if (input == "x")
                return CUSTOMER_MENU;
            
            int day = atoi(input.c_str()); // converts input to integer
            
            // check if in range and is available
            if (day > 0 && --day < DAY_COUNT && check_availability(cache, CONSULTATION, day, -1, cache.booking.expert)) {
                cache.booking.day = day;
                return BOOK_SELECT_SERVICE;
            }
            
            validation = false;
            return state;
        }
        
        
        case BOOK_SELECT_SERVICE:
            if (cache.booking.service == -1) { // skips selection if already selected
                print_booking_status(cache.booking, false);
                
				Option options[SERVICE_COUNT + 1] = {};
				
				for (const Service &service: SERVICES)
					options[option_count++].description = "Service " + service.name;
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return CUSTOMER_MENU;
				
                cache.booking.service = response;
			}
            
            {
                bool consultation = check_availability(cache, CONSULTATION, cache.booking.day, -1, cache.booking.expert);
                bool treatment = check_availability(cache, TREATMENT, cache.booking.day, -1, cache.booking.expert);
                
                if (consultation && treatment) // if both consultation and treatment is available to book
                    return BOOK_SELECT_SERVICE_TYPE; // allows user to select
                else
                    return treatment ? BOOK_TREATMENT : BOOK_CONSULTATION; // otherwise skips selection
            }
        
        
        case BOOK_SELECT_SERVICE_TYPE:
			options = BOOK_SELECT_SERVICE_TYPE_OPTIONS;
			option_count = sizeof(BOOK_SELECT_SERVICE_TYPE_OPTIONS) / sizeof(Option);
            
            print_booking_status(cache.booking, false);
            
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
                    return CUSTOMER_MENU;
                
                validation = check_availability(cache, TREATMENT, cache.booking.day, response, cache.booking.expert);
                
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
                    return CUSTOMER_MENU;
                
                validation = check_availability(cache, CONSULTATION, cache.booking.day, response, cache.booking.expert);
                
                if (!validation)
                    return state;
				
                cache.booking.service_type = CONSULTATION;
                cache.booking.hour = response;
                return BOOK_SELECT_EXPERT;
			}
        
        
        case BOOK_SELECT_EXPERT:
            if (cache.booking.expert == -1) { // skips expert selection if already selected
                print_schedule(cache);
                
                cout << endl << "Experts available for " << format_booking_time(cache.booking) << endl;
                // prints experts available to select
                for (int expert = 0; expert < EXPERT_COUNT; expert++)
                    if (check_availability(cache, cache.booking.service_type, cache.booking.day, cache.booking.hour, expert))
                        cout << "Expert " << EXPERTS[expert].name << endl;
                
				Option options[EXPERT_COUNT + 1] = {};
                option_count = EXPERT_COUNT;
				
				for (int i = 0; i < option_count; i++)
					options[i].description = "Expert " + EXPERTS[i].name;
				
                options[option_count].description = "Cancel";
                
				int response = input_options(options, option_count + 1, validation);
				
				if (!validation)
					return state;
                
                if (response == option_count)
                    return CUSTOMER_MENU;
				
                validation = check_availability(cache, cache.booking.service_type, cache.booking.day, cache.booking.hour, response);
                
                if (!validation)
                    return state;
                
                cache.booking.expert = response;
			}
            
            return BOOK_CONFIRM;
        
            
        case BOOK_CONFIRM:
			options = BOOK_CONFIRM_OPTIONS;
			option_count = sizeof(BOOK_CONFIRM_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            const User &customer = cache.user;
            
            cout << "Please confirm that the following information are correct" << endl;
            print_line();
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
			options = BOOK_PAYMENT_OPTIONS;
			option_count = sizeof(BOOK_PAYMENT_OPTIONS) / sizeof(Option);
            
            cout << "Select your payment option" << endl;
            break;
        
        
        case BOOK_SUCCESS:
			options = BOOK_SUCCESS_OPTIONS;
			option_count = sizeof(BOOK_SUCCESS_OPTIONS) / sizeof(Option);
        {
            const Service &service = SERVICES[cache.booking.service];
            
            string id = "B" + to_string(cache.new_booking_id);
            
            Booking &booking = cache.booking;
            ExpertSchedule &schedule = cache.experts[cache.booking.expert];
            
            // fills expert's schedule with the booking id
            (booking.service_type == TREATMENT ? schedule.treatment_schedule : schedule.consultation_schedule)[booking.day][booking.hour] = id;
            
            booking.customer_username = cache.user.username;
            
            add_booking(id, cache.booking);
            
            // fills schedule
            for (int i = 0; i < (booking.service_type == TREATMENT ? 2 : 1); i++)
                cache.schedule[cache.booking.day][cache.booking.hour + i][cache.booking.expert] = true;
            
            cout << "Successfully booked the appointment!" << endl;
            cout << endl << "Appointment id: B" << cache.new_booking_id++ << endl;
            cout << (booking.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_booking_time(cache.booking) << endl;
            cout << "Expert: " << EXPERTS[booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            
            save_cache(cache);
            
            break;
        }
        
        
        case APPOINTMENTS:
			options = APPOINTMENTS_OPTIONS;
			option_count = sizeof(APPOINTMENTS_OPTIONS) / sizeof(Option);
        {
            string id;
            Booking appointment;
            
            ifstream in;
            in.open(FILES.BOOKING);
            
            cout << "List of appointments:" << endl;
            
            // prints all appointments booked by this user
            while (read_booking(in, id, appointment)) // loops all appointments
                if (appointment.customer_username == cache.user.username) // checks if booked by this user
                    cout << id << endl;
            
            in.close();
            
            cout << endl << "Please enter the appointment id: ";
            cin >> id;
            cout << endl;
            
            if (find_appointment(id, appointment)) { // if appointment found
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
                
                for (const User &expert : EXPERTS)
                    if (username == expert.username) {
                        cache.user = expert;
                        username_found = true;
                        break;
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
            
            cache.user = {}; // clear out cache for security purpose
            break;
        }
            
            
        case STAFF_MENU:
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
			options = SCHEDULE_VIEW_OPTIONS;
			option_count = sizeof(SCHEDULE_VIEW_OPTIONS) / sizeof(Option);
        {
            if (cache.user.username == ADMIN.username) // unavailable for admin
                return INVALID_USER;
            
            int index = 0;
            
            for (const User &expert : EXPERTS) // finds expert index of the current user
                if (expert.username == cache.user.username)
                    break;
                else
                    index++;
            
            int day_offset = cache.schedule_view_week * 7;
            int day_count = cache.schedule_view_week == 4 ? 2 : 7;
            
            const ExpertSchedule &schedule = cache.experts[index];
            
            const int LABEL_SIZE = 5 + 1 + 5;
            const int CELL_SIZE = 5;
            const int LINE_WIDTH = LABEL_SIZE + 7 * (CELL_SIZE + 1);
            
            string s = format_date(cache.schedule_view_week * 7) + " ~ " + format_date(cache.schedule_view_week * 7 + day_count - 1);
            
            center(LINE_WIDTH, s) << endl << endl; // display week information
            
            fill_cout(' ', LABEL_SIZE);
            
            for (int day = 0; day < day_count; day++) {
                cout << '|';
                center(5, SHORT_DAYS_OF_A_WEEK[day]);
            }
            
            cout << endl;
            
            for (int hour = 0; hour < WORK_HOUR_COUNT; hour++) {
                fill_cout('-', LABEL_SIZE); // prints table border
                
                for (int j = 0; j < day_count; j++) {
                    cout << '+'; // prints line intersection
                    fill_cout('-', CELL_SIZE);
                }
                
                cout << endl << OPENING_HOUR + hour << ":00~" << OPENING_HOUR + hour + 1 << ":00"; // prints row label
                
                for (int day = 0; day < day_count; day++) {
                    const string *id = nullptr;
                    const string (&day_schedule)[WORK_HOUR_COUNT] = (cache.booking.service_type == TREATMENT ? schedule.treatment_schedule : schedule.consultation_schedule)[day_offset + day];
                    
                    if (!day_schedule[hour].empty())
                        id = &day_schedule[hour];
                    
                    // if viewing treatment schedule, also check previous hour
                    if (cache.booking.service_type == TREATMENT && hour > 0 && !day_schedule[hour - 1].empty())
                        id = &day_schedule[hour - 1];
                    
                    cout << '|';
                    
                    if (id == nullptr) // no booking
                        center(5, "-");
                    else
                        cout << *id;
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
			options = CUSTOMER_VIEW_OPTIONS;
			option_count = sizeof(CUSTOMER_VIEW_OPTIONS) / sizeof(Option);
            
            if (cache.customer_count > 0) {
                cout << "Customers (Page " << cache.customer_view_page + 1 << " of " << (cache.customer_count + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE << ')' << endl;
                print_line();
                
                int offset = cache.customer_view_page * CUSTOMER_ENTRIES_PER_PAGE;
                
                ifstream in;
                User customer;
                
                in.open(FILES.CUSTOMER);
                
                for (int i = 0; i < offset; i++) // skips all customers in previous pages
                    read_customer(in, customer);
                
                // reads customer datas until page limit or end of file
                for (int i = 0; i < CUSTOMER_ENTRIES_PER_PAGE && read_customer(in, customer); i++)
                    cout << offset + i + 1 << ". " << customer.name << " (" << (customer.gender == MALE ? "Male" : "Female") << ") Phone number: " << customer.phone_number << endl;
                
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
			options = SALES_REPORT_OPTIONS;
			option_count = sizeof(SALES_REPORT_OPTIONS) / sizeof(Option);
            
            {
                cout << "Sales report" << endl;
                print_line();
                
                double expert_sales[EXPERT_COUNT] = {}; // how much sales each expert has made
                int expert_hours[EXPERT_COUNT] = {}; // how much hours each expert has worked
                int service_sales[SERVICE_COUNT] = {}; // how many times a service is selected
                int treatment = 0, consultation = 0; // how many times treatment or consultation is selected
                double most_sales = 0; // the most sales an expert has made
                int popular_service = 0; // the number of times the most popular service is selected
                
                ifstream in;
                string id;
                Booking booking;
                
                in.open(FILES.BOOKING);
                
                while (read_booking(in, id, booking)) { // loop all appointments
                    const Service &service = SERVICES[booking.service];
                    
                    expert_sales[booking.expert] += booking.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee;
                    expert_hours[booking.expert] += booking.service_type == TREATMENT ? 2 : 1;
                    service_sales[booking.service]++;
                    
                    if (booking.service_type == TREATMENT)
                        treatment++;
                    else
                        consultation++;
                    
                    if (expert_sales[booking.expert] > most_sales)
                        most_sales = expert_sales[booking.expert];
                    
                    if (service_sales[booking.service] > popular_service)
                        popular_service = service_sales[booking.service];
                }
                
                in.close();
                
                cout << "Experts Summary" << endl << endl;
                
                for (int expert = 0; expert < EXPERT_COUNT; expert++) // prints expert_sales
                    cout << expert + 1 << ". Expert " << EXPERTS[expert].name << " has earned RM " << setprecision(2) << fixed << expert_sales[expert] << " and worked a total of " << expert_hours[expert] << " hours" << endl;
                
                cout << endl << "Expert(s) that earned most (RM " << most_sales << "):" << endl;
                
                for (int expert = 0; expert < EXPERT_COUNT; expert++) // prints all experts that has earned most_sales
                    if (expert_sales[expert] == most_sales)
                        cout << EXPERTS[expert].name << endl;
                
                print_line();
                
                cout << "Service Summary" << endl;
                
                for (int service = 0; service < SERVICE_COUNT; service++) // prints service_sales
                    cout << service + 1 << ". Service " << SERVICES[service].name << " has been chosen for " << service_sales[service] << " time(s)" << endl;
                
                cout << endl << "Most popular service(s) that has been chosen " << popular_service << " time(s):" << endl;
                
                for (int service = 0; service < SERVICE_COUNT; service++) // prints all services that is selected as much as popular_service
                    if (service_sales[service] == popular_service)
                        cout << SERVICES[service].name << endl;
                
                cout << endl;
                cout << "Total treatments: " << treatment << endl;
                cout << "Total consultations: " << consultation << endl;
            }
            break;
        
        
        case INVALID_USER:
			options = INVALID_USER_OPTIONS;
			option_count = sizeof(INVALID_USER_OPTIONS) / sizeof(Option);
            cout << "Unavailable to this user" << endl;
            break;
            
            
        case LOGOUT:
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

// Entry function. Setups data and process ui loop
int main() {
    int state = MAIN_MENU;
    Cache cache {};
    bool validation = true;
    
    // cache initialization
    cache.booking.day = -1;
    cache.booking.hour = -1;
    cache.booking.service = -1;
    cache.booking.expert = -1;
    cache.new_booking_id = 1000;
    
    read_cache(cache);
    
    // loops ui
    while ((state = ui(state, validation, cache)) != EXIT);
    
    // displays exit message
    ui(EXIT, validation, cache);
    
    save_cache(cache);
    
    return 0;
}
