/*
C++17 Features Demonstrated with Functions and Explanation
*/
#include <iostream>
#include <tuple>
#include <map>
#include <list>
#include <vector>
#include <optional>
#include <variant>
#include <any>
#include <mutex>
#include <functional>
#include <algorithm>
#include <tuple>
#include <string_view>
#include <sstream>
#include <cstddef>
#include <cstring>
#include <execution> // For parallel execution policies

using namespace std;

/*Start from main.*/
int main();

/*List of all features functions*/
void learnStructuralBinding();
void learnFallthrough();
void learnInlineVariable();
void learnConstExprIf();
void learnTemplateArgDeductionClass();
void learnConstexprLambda();
void learnCaptureThisByVAlueInLambda();
void learnNodiscardMaybeUnusedAttribute();;
void learnNestedNamespaces();
void learnOptional();
void learnVariant();
void learnAnyContainer();
void learnFoldingExpression();
void learnSelectAndIfwithInitializer();
void learnApplyFeature();
void learnInvokeFeatures();
void learnStringView();
void learnByte();
void learnSplicingForMapAndSet();
void learnParallelAlgorithm();

/* Entry point function */
int main(){
    learnStructuralBinding();
    return 0;
}

/*
####################################################################################
Structured bindings in C++17 allow you to(auto [x,y,z] = expr) unpack the elements 
of a tuple-like object into individual variables. This can make code more readable 
and can be particularly useful when working with functions or classes that return 
multiple values.
Tuple like object include std::tuple, std::pair, std::array and aggregate data structures.  
*/
tuple<int,double,string> getPerson(){
    return std::make_tuple(22,170.4,"Kedar");
}

using Point = pair<int,int>;
Point getStartPoint(){
    return Point{2,1};
}

void learnStructuralBinding(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    auto [age,height,name] = getPerson();
    cout<<"age : "<<age<<" Height : "<<height<<" Name : "<<name<<endl;

    auto [x,y] = getStartPoint();
    cout<<"x = "<<x<<" y = "<<y<<endl;

    std::map<int,string> numMap = {
        {1,"one"},
        {2,"two"}
    };

    for(const auto& [n,nStr] : numMap )
        cout<<n<<" "<<nStr<<endl;
    learnFallthrough();
}

/*
####################################################################################
 The [[fallthrough]] attribute is used within a switch statement to indicate to the 
 compiler that falling through from one case label to another is intended and not a 
 mistake. This can be useful when you have intentional fall-through behavior that you 
 want to document or when you want to suppress warnings about fall-throughs.
*/
void learnFallthrough(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    char x = 'B';

    switch(x){
        case 'A': 
            cout<<"A"<<endl;
            break;
        case 'B':
            cout<<"B"<<endl;
            //[[fallthrough]]; //intentional 
        case 'C':
            cout<<"C"<<endl;
            break;
        default:
            cout<<"default"<<endl;
    }
    learnInlineVariable();
}

/*
####################################################################################
the inline specifier can be used for variables as well as functions. When applied to 
a variable, it indicates that each definition of the variable is unique and not shared 
between translation units. This can be useful in header files to define variables that 
need to be included in multiple source files without violating the one-definition rule 
(ODR).
Inline variables are particularly useful for defining constants or configuration values 
that need to be shared across multiple source files. They can help reduce code duplication 
and simplify the organization of constants and configuration values in larger projects.
ODR stands for "One Definition Rule." It's a fundamental rule in C++ that states that 
each entity (such as a variable, function, class, or template) must have exactly one 
definition in the entire program

It can also be used to declare and define a static member variable, such that it does not need to
be initialized in the source file.
*/
//it should be defined in header file. Header file is included in multiple source files. 
inline int configVal = 30;
//inline static member
struct S{
   S(): id{count++} {}
   ~S(){count--;}

   int id;
   static inline int count{0};
};
void learnInlineVariable(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    cout<<"configVal = "<<configVal<<endl;
    S s;
    cout<<"id = "<<s.id<<endl;

    learnConstExprIf();
}

/*
####################################################################################
In C++17, constexpr if is a feature that allows you to conditionally compile code 
based on a compile-time condition. It's a more flexible and expressive alternative 
to using preprocessor directives (#if, #ifdef, etc.) or template specialization for 
compile-time branching.
constexpr if is useful when you want to write code that behaves differently based on 
compile-time conditions without using runtime branching. It can lead to more efficient 
code, especially when the different branches have significantly different runtime 
behavior.
*/

template <typename T>
constexpr bool isIntergral(){
    if constexpr (std::is_integral<T>::value){
        //code for integral 
        return true;
    }else{
        //code for non integral
        return false;
    }
}

template <typename T>
void printSize(const T& container){
    if constexpr (std::is_same_v<T,list<int>>){
        cout<<"Size of the list : "<<container.size()<<endl;
    }else{
        cout<<"Size of the container : "<<container.size()<<endl;
    }
}

void learnConstExprIf(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    static_assert(isIntergral<int>() == true);
    static_assert(isIntergral<char>() == true);
    static_assert(isIntergral<float>() == false);

    list<int> intList = {4,7,2,9};
    printSize(intList);

    string s{"Bhushan"};
    printSize(s);

    learnTemplateArgDeductionClass();
}

/*
####################################################################################
Template Argument Deduction for Class Templates, introduced in C++17, allows you to 
deduce template arguments from the constructor arguments, similar to template argument 
deduction for function templates.
Template argument deduction for class templates simplifies the code by allowing the 
compiler to automatically determine the types of template arguments based on constructor 
arguments, reducing the need for explicit type specifications.
*/
template <typename T1, typename T2>
class Pair{
    T1 first;
    T2 second;
public:
Pair(const T1& f, const T2& s): first(f),second(s){}
void print() const {
    cout<<"First : "<<first<<" Second : "<<second<<endl;
}
};

void learnTemplateArgDeductionClass(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    Pair p1(12,34.65);
    p1.print();

    Pair p2('A',string("C++17"));
    p2.print();

    learnConstexprLambda();
}

/*
####################################################################################
constexpr lambdas allow to create lambda expressions that can be evaluated at compile 
time, providing more flexibility in writing compile-time computations and constexpr 
functions. This feature enables you to use lambdas in contexts that require constant 
expressions, such as template arguments or array sizes.
Using constexpr lambdas allows you to perform complex computations at compile time, 
providing efficiency benefits and enabling more advanced compile-time programming 
techniques in C++.
*/
void learnConstexprLambda(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;
    /*The first constexpr specifies that the lambda function itself is constexpr, 
      meaning that it can be evaluated at compile time.
      The second constexpr specifies that the lambda function's operator() 
      (the function call operator) is constexpr.
    */
    constexpr auto factorial = [](int n) constexpr {
        int result = 1;
        for(int i = 1; i <= n; i++){
            result *=i;
        }
        return result;
    };

    constexpr int value = factorial(5);
    cout<<"Factorial of 5 is "<<value<<endl;

    learnCaptureThisByVAlueInLambda();
}

/*
####################################################################################
Capturing this by value in a lambda expression allows you to access member variables 
and member functions of the enclosing class within the lambda body. This is useful 
when you want to use member variables or member functions within the lambda without 
having to explicitly pass them as arguments.
Prior to C++17 Captering this in lambda was reference-only. This is problematic when
lambda is used as callbacks especially in asynchronous code.  
*/
class ValueMultiplier{
    int _factor;
public:
    ValueMultiplier(int factor):_factor(factor){}
    
    auto mutiplyValue(){
        return [this](int& value){
            value *=_factor;
        };
    }

    ~ValueMultiplier()
    {
        cout<<"ValueMultiplier object destroyed"<<endl;
    }
};
void learnCaptureThisByVAlueInLambda(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    function<void(int&)> lambda;
    
    {
        ValueMultiplier obj(2);
        lambda = obj.mutiplyValue();
    }
    //obj is destoyed here but lamda has it's copy
    vector<int> vec ={2,8,5,7,2};
    for_each(vec.begin(),vec.end(),lambda);

    for(auto v : vec){ cout<<v<<' ';}
    cout<<endl;

    learnNodiscardMaybeUnusedAttribute();
}

/*
####################################################################################
The [[nodiscard]] attribute is used to indicate that the return value of a function 
should not be discarded. When a function is marked with [[nodiscard]] and its return 
value is ignored, the compiler will generate a warning, reminding the programmer to 
handle the return value appropriately.
Using the [[nodiscard]] attribute can help prevent bugs and improve code readability 
by ensuring that return values that should not be ignored are properly handled.
[[maybe_unused]] indicates to the compiler that a variable or parameter might be unused
and is intended.
*/
[[nodiscard]] int computeDivision(int num, int den){
    if(den == 0){
        return -1;
    }
    return num/den;
}

void learnNodiscardMaybeUnusedAttribute(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    [[maybe_unused]] int j = 20; //Avoid warning for unused variable if -Wall option is used
    int result = computeDivision(20,0);//Warnig if return value is ignored
    cout<<"Result = "<<result<<endl;

    learnNestedNamespaces();
}

/*
####################################################################################
Using the namespace resolution operator to create nested namespace definitions.
*/
namespace A::B::C{
    int num;
}
void learnNestedNamespaces(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    A::B::C::num = 100;
    cout<<"Nested namespace A::B::C::num = "<<A::B::C::num<<endl;

    learnOptional();
}

/*
####################################################################################
std::optional is a template class that represents an optional (nullable) value. It 
can hold either a value of a given type or no value (null). This is particularly useful 
when a function can fail to produce a result and you want to represent that possibility 
without using pointers or other sentinel values. 
Using std::optional in this way allows you to handle optional values in a more expressive 
and type-safe manner, avoiding the need for error codes or sentinel values.
*/
std::optional<int> convertStringToInt(const string& str){
    try{
        return stoi(str);
    }catch(...){
        return std::nullopt;
    }
}

std::optional<pair<int,int>> divide(int num, int den){
    if(den != 0){
        return std::make_pair(num/den,num%den);
    }else{
        return std::nullopt;
    }
}
//Optional as argument
void printMessage(const std::string& message, 
                const std::optional<std::string>& prefix = std::nullopt) {
    if (prefix.has_value()) {
        std::cout << *prefix << ": " << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}
void learnOptional(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    std::string str = "465";
    auto result = convertStringToInt(str);
    if(result.has_value()){
        cout<<"Convert string value : "<<result.value()<<endl;
    }else{
        cout<<"Failed to convert value"<<endl;
    }

    int num = 14, den = 4;
    auto resultDiv = divide(num,den);
    if(resultDiv.has_value()){
        auto [quotient,reminder] = resultDiv.value();
        cout<<"Quotient: "<<quotient<<" Reminder: "<<reminder<<endl;
    }else{
        cout<<"Divid by zero"<<endl;
    }

    printMessage("Hello");
    printMessage("World", "Hello");

    learnVariant();
}
/*
####################################################################################
std::variant is a type-safe union that can hold values of different types. It provides 
a way to represent a value that can be one of several alternative types. This can be 
useful in situations where you need to handle different types of values, such as in 
parsers, state machines, or variant visitors.
std::variant provides a flexible and type-safe way to work with values that can be of 
different types, making it easier to handle complex scenarios where the types are not 
known at compile time.
*/
//Lets implement add function for int and double

using Operand = std::variant<int,double>;

Operand add(const Operand& op1,const Operand& op2){
    if(std::holds_alternative<int>(op1) && std::holds_alternative<int>(op1)){
        return std::get<int>(op1) + std::get<int>(op1);
    }else if(std::holds_alternative<double>(op1) && std::holds_alternative<double>(op2)){
        return std::get<double>(op1) + std::get<double>(op2);
    }else{
        throw std::runtime_error("Invalid operation for addition");
    }
}
void learnVariant(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    try{
        Operand op1 = 20;
        Operand op2 = 14.5;

        Operand result = add(op1,op2);

        if(std::holds_alternative<int>(result)){    
            cout<<"Result int : "<<std::get<int>(result)<<endl;
        }else if(holds_alternative<double>(result)){
            cout<<"Result double : "<<std::get<double>(result)<<endl;
        }
    }catch(const std::exception& e){
        cerr<<"Error : "<<e.what()<<endl;
    }

    learnAnyContainer();
}
/*
####################################################################################
std::any is a type-safe container that can hold any type of value. It is similar to 
void* in C, but with type safety and support for value semantics. std::any can be 
useful in situations where you need to store a value of an unknown or varying type, 
such as in generic programming or when working with heterogeneous containers.
std::any provides a flexible way to store and retrieve values of unknown or varying 
types, making it useful for implementing generic or polymorphic behavior in C++ programs.
*/
void learnAnyContainer(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    std::any value;
    value = 20;
    if(value.has_value())
        cout<<value.type().name()<<" value = "<<any_cast<int>(value)<<endl;

    value = 12.5;
    cout<<value.type().name()<<" value = "<<any_cast<double>(value)<<endl;

    value.reset();
    if(!value.has_value()){
        cout<<"value is empty"<<endl;
    }

    try{
        value = string("Test C++ std::any");
        cout<<any_cast<int>(value);
    }catch(const std::bad_any_cast& e){
        cerr<<"Error : "<<e.what()<<endl;
    }
    
    auto a = make_any<string>("Test C++ std::any");
    cout<<a.type().name()<<" a = "<<any_cast<string>(a)<<endl;

    learnFoldingExpression();
}
/*
####################################################################################
folding expressions allow you to apply a binary operator to a pack of arguments, 
producing a single result. This provides a concise way to perform operations on 
variadic templates, such as combining values in a pack or applying operations to 
every element in a pack.

*/
template <typename... Args>
constexpr auto sum(Args... args){
    return (... + args);
}

template <typename T,typename... Args>
constexpr void push_back_v(vector<T>& vec,Args&&... args){
    (vec.push_back(forward<Args>(args)),...);
}
//applying operation on every element
template<typename... Args>
constexpr bool allGreaterThan(int threshold, Args... args){
    return (... && (args > threshold));
}

void learnFoldingExpression(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    cout<<sum(1,2,3)<<endl;

    vector<int> vec;
    push_back_v(vec,1,4,5,6);
    for(auto elem : vec) cout<<elem<<' ';
    cout<<endl;
    cout<<boolalpha;
    constexpr bool result = allGreaterThan(10,12,34,25);
    cout<<result<<endl;

    learnSelectAndIfwithInitializer();
}
/*
####################################################################################
In C++17, the if and switch statements can include an initializer in the condition or 
in the switch statement itself, allowing you to declare and initialize variables within 
the statement itself. This can help improve code readability and reduce the scope of 
variables to where they are actually needed.
Using initializers in if and switch statements can be beneficial when you need to 
declare and initialize variables that are only needed within the statement's block. 
It helps to keep the code more concise and reduces the scope of variables, making the 
code easier to understand and maintain.
*/
void learnSelectAndIfwithInitializer(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    int x = 2;

    switch(int y = x * 2; y){
        case 1: cout<<"y is one"<<endl;break;
        case 2: cout<<"y is two"<<endl;break;
        default:cout<<"y is neither 1 or 2"<<endl;
    }   
//y is outof scope here
    int number = 9;
    if(int reminder = number%2; reminder == 0)
        cout<<number<<" number is even"<<endl;
    else
        cout<<number<<" number is odd"<<endl;

    std::mutex mx;
    vector<int> vec;
    //mutex mx scope is in if block only
    if(lock_guard<mutex> lock(mx); vec.empty()){
        vec.push_back(10);
    }    
    //mx is unlocked here

    learnApplyFeature();
}
/*
####################################################################################
the std::apply function allows you to invoke a function with a tuple of arguments, 
unpacking the tuple into individual arguments for the function call. This can be useful 
-in situations where you have a function that accepts multiple arguments, and you want to 
pass these arguments as a tuple.
*/
void learnApplyFeature(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    auto sum = [](int a, int b){
        return a + b;
    };
    tuple<int,int> intTuple(10,20);
    cout<<apply(sum,intTuple)<<endl;

    learnInvokeFeatures();
}
/*
####################################################################################
The std::invoke function template allows you to invoke any callable object (function, 
function pointer, member function pointer, functor, lambda, etc.) with a set of arguments. 
It provides a uniform interface for invoking callable objects, regardless of their type.

template<typename F, typename... Args>
decltype(auto) invoke(F&& f, Args&&... args) {
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

*/

void callback(int value){
    cout<<"Callback invoked with value : "<<value<<endl;
}
struct Foo{
    void MemberFunction(int x, int y){
        cout<<"Struct member function called with : "<<x<<", "<<y<<endl;
    }
};
void learnInvokeFeatures(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    invoke(callback,5);

    Foo foo;
    //member function in fuction object 
    //std::function<void(Foo&, int, int)> func = &Foo::memberFunction; 
    //invoke(func,foo,20,40);
    invoke(&Foo::MemberFunction,foo,20,40);

    learnStringView();
}

/*
####################################################################################
std::string_view is a non-owning view into a sequence of characters. It provides a 
lightweight, read-only view of a string, without requiring memory allocation or copying. 
std::string_view is particularly useful for functions and APIs that need to work with 

strings without taking ownership or making unnecessary copies.
std::string_view does not require a null terminator, making it suitable for working with 
substrings or binary data
std::string_view is used to efficiently work with strings in various scenarios, 
demonstrating its utility in real-world applications.
*/

void learnStringView(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    string_view str1 = "Hello, string_view";
    //str2 holds same view, no extra copy of Hello, string_view
    string_view str2(str1);
    
    cout<<"str1 : "<<str1<<" "<<"str2 : "<<str2<<endl;

    string s{"Test string_view"};
// When initializing a string_view using a string, ensure that
// the string object remains alive for the lifetime of the string_view. 
    string_view str3(s);
    cout<<"Str2 length : "<<str3.length()<<endl;
    cout<<str3.substr(5,6)<<endl;
    cout<<"Contains Test : "<<boolalpha<< (str3.find("Test") != string_view::npos)<<endl;
    cout<<"Compares : "<<str3.compare(str2)<<endl;

    learnByte();
}
/*
####################################################################################
std::byte is a new data type introduced to represent a byte of data. It is 
part of the <cstddef> header and is designed to provide a type-safe alternative to 
using unsigned char or other integer types for raw byte manipulation.
Only operator overload available are bitwise operataions. 
*/

struct Data{
    int id;
    float value;
};

vector<std::byte> serialize(const Data& data){
    vector<std::byte> buffer(sizeof(Data));
    std::memcpy(buffer.data(),&data,sizeof(Data));
    return buffer;
}

Data deserialize(const vector<std::byte>& buffer){
    Data data;
    std::memcpy(&data,buffer.data(),sizeof(Data));
    return data;
}

void learnByte(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    std::byte b{0x7f};
    cout<<to_integer<int>(b)<<endl;

    Data d1{10,15.4};

    Data d2 = deserialize(serialize(d1));
    cout<<"id : "<<d2.id<<" value : "<<d2.value<<endl;

    learnSplicingForMapAndSet();
}
/*
####################################################################################
C++17 introduced the ability to "splice" nodes between std::map and std::set containers. 
Splicing is a powerful feature that allows for transferring elements between containers 
without reallocating or copying the elements themselves. This can be very efficient, as 
it avoids the overhead of element construction and destruction.
The ability to move elements without reallocation or copying can significantly improve 
the performance of programs that require frequent modifications to container elements.
*/
void learnSplicingForMapAndSet(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;

    map<int,string> map1 = {{1,"one"},{2,"two"},{3,"three"}};
    map<int,string> map2 = {{4,"four"},{5,"five"}};
    auto itr = map1.find(2);
    map2.insert(map1.extract(itr));
    //map2.insert(map1.extract(2));
    for(const auto& [key,value] : map2){
        cout<<"key : "<<key<<" value : "<<value<<endl;
    }

    learnParallelAlgorithm();
}
/*
####################################################################################
C++17 introduced parallel algorithms as part of the Standard Library, allowing 
developers to take advantage of modern multi-core processors to perform computations 
more efficiently. Parallel algorithms in C++17 provide a straightforward way to 
parallelize standard algorithms like std::sort, std::for_each, and many others.
The three execution policies provided are:
    std::execution::seq         : Sequential execution (default behavior).
    std::execution::par         : Parallel execution.
    std::execution::par_unseq   : Parallel and vectorized execution.
*/
void learnParallelAlgorithm(){
    cout<<"--------------"<<__func__<<"--------------"<<endl;
    
    vector<int> vec(10);

    std::generate(vec.begin(),vec.end(),std::rand);

    cout<<"Orignal vector : ";
    for(auto val : vec)
        cout<<val<<' ';
    cout<<endl;
 
    sort(std::execution::par,vec.begin(),vec.end());
 
    cout<<"Sorted vector  : ";
    for(auto val : vec)
        cout<<val<<' ';
    cout<<endl;
}
 