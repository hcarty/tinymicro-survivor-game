/**
 * @file tinymicro.h
 * @date 30-May-2023
 */

#ifndef __tinymicro_H__
#define __tinymicro_H__

#define __NO_SCROLLED__
#include "Scroll.h"

/** Game Class
 */
class tinymicro : public Scroll<tinymicro>
{
public:


private:

                orxSTATUS       Bootstrap() const;

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();


private:
};

#endif // __tinymicro_H__
