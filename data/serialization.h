/*
 * FileName: data/serialization.h
 * Author: 陈若珂
 * Description: 提供对数据（反）序列化相关操作的支持
 * */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

/*
 * Name: LinkedString
 * Description: 表示通过链表实现的字符串
 * */
struct LinkedString {
    // 字符串分块
    const char *chunk;

    // 指向下一个结点
    LinkedString *next;
};

typedef LinkedString *LinkedStringPointer;

/*
 * Name: SerializationCallback
 * Description: 表示完成序列化后的回调函数
 * */
typedef void (*SerializationCallback)();

/*
 * Name: SerializationHelper
 * Description: 表示将对象转换为序列化字符串的方法
 * Arguments:
 *     source: 指向要进行序列化的对象的指针
 *     serializedData: 指向用于存放序列化字符串的LinkedString对象的指针
 * Returns: 序列化字符串的长度
 * */
typedef int (*SerializationHelper)(const void *source, LinkedStringPointer serializedData);

/*
 * Name: UnserializationHelper
 * Description: 表示将序列化字符串转换为对象的方法
 * Arguments:
 *     destination: 指向用于存放反序列化结果的对象的指针
 *     serializedData: 指向存放了序列化字符串的LinkedString对象的指针
 * */
typedef void (*UnserializationHelper)(void *destination, LinkedStringPointer serializedData);

/*
 * Name: SerializationContext
 * Description: 表示序列化操作的上下文信息
 * */
struct SerializationContext {
    // 序列化方式
    SerializationHelper helper;

    // 序列化回调
    SerializationCallback callback;

    // 文件路径
    const char* filePath;
};

/*
 * Name: UnserializationContext
 * Description: 表示反序列化操作的上下文信息
 * */
struct UnserializationContext {
    // 反序列化方式
    UnserializationHelper helper;

    // 文件路径
    const char* filePath;
};



/*
 * Name: Serialize
 * Description: 通过指定的序列化上下文对对象进行序列化
 * Arguments:
 *     source: 指向要进行序列化的对象的指针
 *     context: 序列化上下文
 * Returns: 若序列化成功，返回0
 *          否则，返回-1
 * */
int Serialize(const void *source, const SerializationContext *context);

/*
 * Name: Unserialize
 * Description: 通过指定的反序列化上下文对对象进行反序列化
 * Arguments:
 *     destination: 指向要存放序列化结果的对象的指针
 *     context: 反序列化上下文
 * Returns: 若反序列化成功，返回0
 *          若文件不存在，返回1
 *          否则，返回-1
 * */
int Unserialize(void *destination, const UnserializationContext *context);

#endif
