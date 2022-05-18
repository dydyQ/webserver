/**
 * @file Singleton.h
 * @brief 单例模式封装
 */

#ifndef SYLARWEBSERVER_SINGLETON_H
#define SYLARWEBSERVER_SINGLETON_H

#include <memory>

namespace sylar {
    namespace {
        template<class T, class X, int N>
        T &GetInstanceX() {
            static T v;
            return v;
        }

        template<class T, class X, int N>
        std::shared_ptr<T> GetInstancePtr() {
            static std::shared_ptr<T> v(new T);
            return v;
        }
    }

    /**
     * @brief 单例模式封装类
     * @details T 类型
     * X 为了创造多个实例对应的Tag
     * N 同一个Tag创造多个实例索引
     */
    template<class T, class X = void, int N = 0>
    class Singleton {
    public:
        static T *GetInstance() {
            static T v;
            return &v;
        }
    };
}
#endif //SYLARWEBSERVER_SINGLETON_H
