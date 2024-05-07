//C++14 Features all in one

#include <iostream>
#include <functional>
#include <memory>
#include <chrono>

using  namespace std;
/*
####################################################################################
*/
void learnIntegerSequence(){

}
/*
####################################################################################
make_unique
In C++14, std::make_unique is a utility function used to create a std::unique_ptr 
with dynamic memory allocation. It is similar to std::make_shared, but it is used 
for creating std::unique_ptr objects. std::make_unique was introduced in C++14 to 
simplify the creation of std::unique_ptr objects and to ensure exception safety
Memory Management: Use std::make_unique to manage dynamic memory allocation and 
deallocation automatically without the need for manual memory management.
Factory Functions: std::make_unique can be used in factory functions to create 
objects dynamically and return them as std::unique_ptr without leaking memory.
Exception Safety: Using std::make_unique ensures exception safety because the memory 
is automatically released if an exception is thrown before the std::unique_ptr is 
assigned to a variable.
Avoid having to use new operator
std::unique_ptr<MyObject> createMyObject(int value) {
    return std::make_unique<MyObject>(value);
}
*/
void learnMakeUnique(){
    unique_ptr<int> ptr = make_unique<int>(40);
    //creates unique pointers to an array of integers
    auto arrPtr = make_unique<int[]>(5);
    arrPtr[0] = 0;arrPtr[1]=1;

    learnIntegerSequence();
}

/*
####################################################################################
User-defined literals (UDLs) allow you to define custom suffixes for literal 
constants. This feature enables you to create your own syntax for constructing 
values of various types. When you define a UDL, you specify a suffix that can be 
appended to a literal value (such as an integer, floating-point number, or string 
literal) to create a value of a specific type. 
you can also use it for custom numeri type. 
*/
/*//custom numeri type
class MyNumber{
private:
   int _value;

public:
MyNumber(int value):_value(value){};

int getValue(){return _value;}
};
//User defined literals for my number
constexpr MyNumber operator"" _myn(unsigned long int value){
    return MyNumber(static_cast<int>)(value));
}*/
//uses defined literals for time duration s
constexpr std::chrono::milliseconds operator"" _ms(unsigned long long ms){
    return  std::chrono::milliseconds(ms);
}
//user defined literals for physical constant
constexpr long double operator"" _meter(long double value){
    return value;
}

void learnUserDefinedLiterals(){
    auto duration = 100_ms;
    cout<<"Duration in milliseconds : "<<duration.count()<<endl;

    auto length = 20.0_meter;  
    cout<<"Length : "<<length<<endl;
//built in literals for chrono              
    using namespace std::chrono_literals;
    auto  day = 24h;
    cout<<"Minute in a day : "<<chrono::duration_cast<chrono::minutes>(day).count()<<endl;
}
/*
####################################################################################
[[depreacated]] attribute
Indicate that unit is discouraged and yeilds compilattion warnings.

*/
[[deprecated]]
void legacyCodeMethod(){
    cout<<"Deprecated legacy method"<<endl;
}

[[deprecated("Use new_method() instead")]]
void legacyCodeMethod2(){
    cout<<"Deprecated legacy method2"<<endl;
}

void learnDeprecatedAttribute(){
  //  legacyCodeMethod();
  //  legacyCodeMethod2();

    learnUserDefinedLiterals();
}

/*
####################################################################################
Variable templates
In C++14, variable templates allow you to define templates that represent variables, 
rather than types. This means you can create templates for values, which can be useful 
for defining constants or parameterizing functions and classes with values.

*/
template <typename T>
constexpr T pi = T(3.14159265358979323846);

template<class T>
constexpr T e = T(2.7182818284590452353);

//template parameterization
template <typename T, T DefaultValue>
class MyClass {
public:
    T getValue() { return DefaultValue; }
};

//type traits
template <typename T>
constexpr bool is_pointer_v = std::is_pointer<T>::value;

void learnVarialbeTemplate(){
    
    cout<<"Pi (float):"<< pi<float> <<endl;
    cout<<"Pi (double):"<< pi<double> <<endl;
    cout<<"Pi (long double):"<< pi<long double> <<endl;

    MyClass<int, 56> intInstance;
    //MyClass<float, 3.14> floatInstane;    //works with C++20
    cout<<"intInstane : "<<intInstance.getValue();
    //cout<<"floatInstance : "<<floatInstane.getValue();
    cout<<boolalpha;
    cout<<"Is int* is a pointer? "<<is_pointer_v<int*><<endl;
    cout<<"Is floatt a pointer? "<<is_pointer_v<float><<endl;

    learnDeprecatedAttribute();
}

/*
####################################################################################
Relaxing contrain on constexpr
 In C++11, constexpr functions were required to have a single return statement and 
 could only contain a limited set of operations. C++14 relaxed these restrictions, 
 allowing constexpr functions to have multiple return statements, loops (including 
 range-based loops), and local variables.
 Variable Declarations in constexpr Functions: C++14 allows the declaration of local 
 variables within constexpr functions, further expanding the capabilities of constexpr 
 functions.
 constexpr Member Functions: In C++14, member functions of classes can be declared as 
 constexpr, allowing them to be evaluated at compile time if their arguments are constant 
 expressions.
 C++14 make constexpr more flexible and easier to use, allowing for more complex compile 
 time computations and enabling more efficient code generation in certain scenarios.
 
 In C++14, the constexpr specifier is used to indicate that a function or object can be 
 evaluated or initialized at compile time. This allows you to perform computations and 
 initialize values at compile time, which can lead to improved performance and efficiency. 
*/
constexpr int factorial(int n){
    if(n <=  1){
        return 1;
    }
    return n*factorial(n-1);
}

void learnConstexpr(){
    //using constexpr function at compile time
    constexpr int result = factorial(5);
    cout<<"Compile time result = "<<result<<endl;
    int y = 4;
    //using constexpr function at run time 
    //called runtime with useser defined value
    int result2 = factorial(y);
    cout<<"Run time result2 = "<<result2<<endl;

    //compile time constant 
    constexpr int arrSize= 5;
    int arr[arrSize]; //size of the array is known at compile type.

    learnVarialbeTemplate();
};
/*
####################################################################################
In C++14, decltype(auto) is a type specifier that deduces the type of a 
variable or expression based on its initializer or return value. 
decltype(auto) is used to deduce the exact type of variable
decltype(auto) is a powerful feature that allows you to write more concise 
and expressive code, especially in template metaprogramming and when dealing 
with complex type deduction scenarios. 

it also works with class member functions. 
*/
//return type is int
auto foo(const int& i){ return i;};
//auto foo(const int& i)->decltype(auto) { return i;};

//return type is const int&
decltype(auto) bar(const int& i){
    return i;
}

void learnDecltypeAuto(){
    cout<<"Function : "<<__func__<<endl;
    const int       x = 10;
    auto            intType = x; //var is int
    decltype(auto)  constIntType = x;

    int  y = 20;
    int& yRef =y; 
    auto y1 = yRef; //y1 is int
    decltype(auto) y2 = yRef; //y

    int&& Zrv = 0;
    auto zInt = Zrv; //zInt is  int
    decltype(auto) Zrv1 = std::move(Zrv); //Zrv1 is rvalue ref int&&

    static_assert(std::is_same<const int&,decltype(foo(y))>::value == 0);
    static_assert(std::is_same<const int&,decltype(bar(y))>::value == 1);
    static_assert(std::is_same<decltype(Zrv),decltype(Zrv1)>::value == 1);
    
    learnConstexpr();
}

/*
####################################################################################
Return type deduction or auto return type or deduced return type
Allow c++14 to infer return type of function from its return statement.
Return type can be deduced using auto statement. 

Factory Functions: Return type deduction can be used in factory functions 
where the return type depends on runtime conditions. For example:

auto createShape(bool isCircle) {
    if (isCircle) {
        return Circle();
    } else {
        return Rectangle();
    }
}
*/
//Fun with auto return type deduction
auto f(int a, int b){ 
    return a + b; //compiler deduce return type as int
};

template<typename T>
auto& ft(T& t){
    return t;
}

auto g = [](auto&  x) ->auto& {
    return ft(x);
};

void returnTypeDeduction(){
    int k = 10;

    int& z = g(k);
    cout<<"K reference = "<<z<<endl;

    learnDecltypeAuto();
}

/*
####################################################################################
In C++14, generic lambda expressions allow you to write lambda functions that 
are templated, meaning they can accept arguments of any type. This can make your 
code more flexible and reduce the need for function overloads. This is useful for 
writing generic code.
Lambda capture initializers, introduced in C++14, allow you to initialize captured 
variables within a lambda expression.
Lambda capture initializers can be useful in various situations where you need to 
capture variables by value in a lambda and initialize them with specific values.

Now it is possible to move values to lambda that could previously be only captured
by copy or reference. move only types can be captured in lambda by value.

reference Capture can have different name than the referenced variable

*/

/*
In functional programming, lambda capture initializers can be used to capture and 
store intermediate values for later use. This can help in creating more expressive 
and readable code.
*/

auto getTableGenerator(int n){
//This would not compile without mutable as by default lambdas are const function
    return [x=0,y=n]() mutable{
        x+=y;
        return x;
    };
}
auto adderFactory(int base){
    return [base](int value){ 
            return value + base;};
}

void lambdaInFunctionalProgramming(){
    auto add5 = adderFactory(5);
    cout<<add5(15)<<endl;
    auto table10 = getTableGenerator(10);
    cout<<table10()<<endl;
    cout<<table10()<<endl;
    cout<<table10()<<endl;

    returnTypeDeduction();
}
/*
When using lambdas as callback functions, you may want to capture certain variables 
by value and initialize them with specific values at the time of defining the lambda. 
*/
void doSomethingAsync(int x, int y, function<void(int)> callback){
    callback(x + y);
}

void lambdaCaptureWithCallback(){
    int x = 10;
    int y = 20;

    doSomethingAsync(x,y, [initialX=x,initialY=y](int result){
            cout<<"Result = "<<result<<endl;
            cout<<"Captured initial x = "<<initialX<<" initial y = "<<initialY<<endl;
        });

    lambdaInFunctionalProgramming();
}

void genericLambdaAndLambdaCaptureInitializer(){
    //Generic Lambda expressoin
    /*the add lambda function is templated using auto for its arguments. This allows 
    add to accept arguments of any type, whether they are integers, floating-point numbers, 
    or even strings. The lambda then returns the sum of the two arguments or concatenates 
    them if they are strings.
    */
    auto add = [](auto a, auto b){
        return a + b;
    };

    cout<<add(3,4)<<endl;
    cout<<add(11.34,12.33)<<endl;
    cout<<add(string("Hello, "),string("World"))<<endl;
    //Lambda capture initializer. 
    /*
    In below example, the lambda [x=x, y=y](){...} captures x and y by value and 
    initializes them with their current values at the point of lambda creation. This 
    means that even if x and y change after the lambda is created, the captured values 
    inside the lambda remain the same.
    */
   int x = 10;
   int y = 20;

   auto lambda = [x=x,y=y](){
    cout<<"Captured Values : x = "<<x<<" y = "<<y<<endl;
   };

   x = 15;
   y = 25;

   lambda();

 //   auto p = make_unique<int>(1);
 //   auto task = [p=move(p)]{ *p = 5; };

    auto j = 1;
    auto f = [&r=j,j=j+10]{
        ++r;
        return r + j;
    };
    ////return 13  and set x=2
    cout<<f()<<endl;

    lambdaCaptureWithCallback();
}


/*
####################################################################################
Binary literals in C++14 provide a more readable and convenient way 
to work with binary numbers in your code. Prior to C++14, representing 
binary numbers required converting them from binary to decimal manually 
or using other methods. With binary literals, you can directly express 
binary numbers in your code, making it easier to understand and maintain.

It is possible to seperate digit with ' for better readability. 
*/
void binaryLiterals(){
    //using binary number to represent number
    //This represent binary number  29
    int binaryNumber = 0b0101'1101;
    cout<<"Binary Number : "<<binaryNumber<<endl;
    //perfroming bitwise operation xor ^
    int result = binaryNumber ^ 0b0110'0110;
    cout<<"Result of bitwise XOR : "<<result<<endl;

    genericLambdaAndLambdaCaptureInitializer();
}


int main(){
    binaryLiterals();
    return 0;
}