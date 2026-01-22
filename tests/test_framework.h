#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <functional>
#include <vector>

//простой тестовый фреймворк
class TestFramework {
private:
    struct TestCase {
        std::string name;
        std::function<void()> testFunc;
    };
    
    std::vector<TestCase> tests;
    int passedTests = 0;
    int failedTests = 0;
    
public:
    static TestFramework& instance() {
        static TestFramework instance;
        return instance;
    }
    
    void addTest(const std::string& name, std::function<void()> testFunc) {
        tests.push_back({name, testFunc});
    }
    
    void runAll() {
        std::cout << "\n🧪 Запуск тестов...\n" << std::endl;
        std::cout << "================================" << std::endl;
        
        for (const auto& test : tests) {
            try {
                std::cout << "▶ " << test.name << "... ";
                test.testFunc();
                std::cout << "✅ PASSED" << std::endl;
                passedTests++;
            } catch (const std::exception& e) {
                std::cout << "❌ FAILED" << std::endl;
                std::cout << "  Причина: " << e.what() << std::endl;
                failedTests++;
            }
        }
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Результаты:" << std::endl;
        std::cout << "  ✅ Пройдено: " << passedTests << std::endl;
        std::cout << "  ❌ Провалено: " << failedTests << std::endl;
        std::cout << "  📊 Всего: " << (passedTests + failedTests) << std::endl;
        
        if (failedTests == 0) {
            std::cout << "\n🎉 Все тесты пройдены!" << std::endl;
        } else {
            std::cout << "\n⚠️  Есть проваленные тесты!" << std::endl;
        }
    }
    
    int getFailedCount() const {
        return failedTests;
    }
};

//макросы для удобства
#define TEST(name) \
    void test_##name(); \
    struct TestRegistrar_##name { \
        TestRegistrar_##name() { \
            TestFramework::instance().addTest(#name, test_##name); \
        } \
    }; \
    static TestRegistrar_##name registrar_##name; \
    void test_##name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Assertion failed: NOT(" #condition ")"); \
    }

#define ASSERT_EQUAL(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    }

#define ASSERT_THROWS(statement) \
    { \
        bool threw = false; \
        try { \
            statement; \
        } catch (...) { \
            threw = true; \
        } \
        if (!threw) { \
            throw std::runtime_error("Expected exception not thrown: " #statement); \
        } \
    }

#define RUN_ALL_TESTS() \
    TestFramework::instance().runAll(); \
    return TestFramework::instance().getFailedCount();

#endif //TEST_FRAMEWORK_H



