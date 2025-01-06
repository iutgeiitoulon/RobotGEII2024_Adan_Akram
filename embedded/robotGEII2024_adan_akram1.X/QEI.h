/* 
 * File:   QEI.h
 * Author: E306_PC2
 *
 * Created on 6 janvier 2025, 13:50
 */

#ifndef QEI_H
#define	QEI_H

#ifdef	__cplusplus
extern "C" {
#endif

    void InitQEI1();
    void InitQEI2();
    void QEIUpdateData();
    void SendPositionData();


#ifdef	__cplusplus
}
#endif

#endif	/* QEI_H */

