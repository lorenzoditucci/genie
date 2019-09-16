#ifndef UTILS_EXCEPTIONS_H_
#define UTILS_EXCEPTIONS_H_

#define DIE(msg) UTILS_DIE(msg)
#define UTILS_DIE(msg) UTILS_THROW_RUNTIME_EXCEPTION(msg)
#define UTILS_THROW_RUNTIME_EXCEPTION(msg) throw utils::RuntimeException(__FILE__, __FUNCTION__, __LINE__, msg)

#include <exception>
#include <iostream>
#include <string>

namespace utils {

class Exception : public std::exception {
   public:
    explicit Exception(std::string msg);

    ~Exception() noexcept override;

    virtual std::string msg() const;

    const char *what() const noexcept override;

   protected:
    std::string msg_;
};

class RuntimeException : public Exception {
   public:
    explicit RuntimeException(const std::string &file, const std::string &function, int line,
                              const std::string &msg) noexcept;

    RuntimeException(const RuntimeException &e) noexcept;

    ~RuntimeException() noexcept override;
};

}  // namespace utils

#endif  // UTILS_EXCEPTIONS_H_
