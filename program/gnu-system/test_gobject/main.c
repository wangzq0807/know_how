#include <stdio.h>
#include "animal_object.h"
#include "bird_object.h"
#include "bird_object.h"

void main()
{
    AnimalObject *animal = g_object_new(ANIMAL_TYPE_OBJECT, NULL);
    animal_object_move(animal, 0, 1);
    BirdObject *bird = g_object_new(BIRD_OBJECT_TYPE, NULL);
    //GObject* aaa = g_object_new(G_OBJECT_TYPE, NULL);
    //AnimalObject *animal1 = ANIMAL_OBJECT(aaa);
    GObject *gobj = G_OBJECT(animal);
    g_print("Hello, World! %d, %d\n", animal, gobj);
}