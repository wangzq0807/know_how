/* -*- coding:utf-8 -*- */
#ifndef BIRD_OBJECT_H
#define BIRD_OBJECT_H
#include <glib-object.h>
#include "animal_object.h"
G_BEGIN_DECLS
//=== 定义类型 ===
/* 类型标识 */
GType bird_object_get_type();
#define BIRD_OBJECT_TYPE (bird_object_get_type())
/* 类型转化 */
#define BIRD_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), BIRD_OBJECT_TYPE, BirdObject))

//=== 类定义 ===
typedef struct _BirdObject {
	AnimalObject parent;
	gint   aaa;
} BirdObject;

typedef struct _BirdObjectClass {
	AnimalObjectClass parent_class;
} BirdObjectClass;

//=== 成员函数 ===
gint
bird_object_fly(BirdObject *obj, gint n, char* sz);

G_END_DECLS

#endif // BIRD_OBJECT_H
