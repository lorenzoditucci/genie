#ifndef GENIE_EXCEPTIONS_H_
#define GENIE_EXCEPTIONS_H_

#define GENIE_DIE(message) GENIE_THROW_RUNTIME_EXCEPTION(message)

#define GENIE_THROW_RUNTIME_EXCEPTION(message) throw lae::RuntimeException(__FILE__, __FUNCTION__, __LINE__, message)

#include <exception>
#include <iostream>
#include <string>

namespace lae {

class Exception : public std::exception {
   public:
    explicit Exception(const std::string& message);

    ~Exception() noexcept override;

    //to review
    Exception(const Exception&) = delete;
    Exception& operator=(const Exception&) = delete;
    Exception(Exception&&) = delete;
    Exception& operator=(Exception&&) = delete;

    virtual std::string message() const;

    const char* what() const noexcept override;

   protected:
    std::string m_message;
};

class RuntimeException : public Exception {
   public:
    explicit RuntimeException(const std::string& file, const std::string& function, int line,
                              const std::string& message) noexcept;

    RuntimeException(const RuntimeException& e) noexcept;

    ~RuntimeException() noexcept override;

    //to review
    RuntimeException(const RuntimeException&) = delete; //CLion gives an error I'm not sure how to correct
    RuntimeException& operator=(const RuntimeException&) = delete;
    RuntimeException(RuntimeException&&) = delete;
    RuntimeException& operator=(RuntimeException&&) = delete;

};

}  // namespace genie

#endif  // GENIE_EXCEPTIONS_H_
