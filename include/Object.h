/**
 * @file Object.h
 * @date 30-May-2023
 */

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "tinymicro.h"

/** Object Class
 */
class Object : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);


private:
};

#endif // __OBJECT_H__
