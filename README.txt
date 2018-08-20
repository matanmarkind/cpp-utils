This is my attempt at writing random things in C++ I think could be
interesting. Projects include:
- Threadpool - Think I want to make this interface simpler so the user just
    passes in a void() function instead of emplacing the contents. Make it
    clear when the user passes the function object they need to think about
    how they want to capture for thread safety.
- DEFAULT_VAL_ENUM - macro which tries to save user from the boilerplate
    involved in writing an enum if you also want a ToString option. Only works
    for default values, you can't explicitly give an entry a value. Couldn't
    get it to compile if had explicit assignment.
- Some other useful threading things to designate ownership over some contents.
