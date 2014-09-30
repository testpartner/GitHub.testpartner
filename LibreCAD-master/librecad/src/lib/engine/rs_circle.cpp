/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2011-2012 Dongxu Li (dongxuli2011@gmail.com)
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!
**
**********************************************************************/

#include <QDebug>
#include "rs_circle.h"

//#include <values.h>

#include "rs_constructionline.h"
#include "rs_information.h"
#include "rs_graphicview.h"
#include "rs_painter.h"
#include "rs_linetypepattern.h"
#include "lc_hyperbola.h"
#include "lc_quadratic.h"



/**
 * Default constructor.
 */
RS_Circle::RS_Circle(RS_EntityContainer* parent,
                     const RS_CircleData& d)
    :RS_AtomicEntity(parent), data(d) {
    calculateBorders();
}



void RS_Circle::calculateBorders() {
    RS_Vector r(data.radius,data.radius);
    minV = data.center - r;
    maxV = data.center + r;
}



/**
 * @return Angle length in rad.
 */
double RS_Circle::getAngleLength() const {
    return 2*M_PI;
}



/**
 * @return Length of the circle which is the circumference.
 */
double RS_Circle::getLength() const {
    return 2*M_PI*data.radius;
}

bool RS_Circle::isTangent(const RS_CircleData&  circleData){
    double d=circleData.center.distanceTo(data.center);
    if( fabs(d-fabs(circleData.radius - data.radius))<RS_TOLERANCE ||
            fabs(d-fabs(circleData.radius + data.radius))<RS_TOLERANCE ) return true;
    return false;
}


/**
 * Creates this circle from a center point and a radius.
 *
 * @param c Center.
 * @param r Radius
 */
bool RS_Circle::createFromCR(const RS_Vector& c, double r) {
    if (fabs(r)>RS_TOLERANCE && c.valid ) {
        data.radius = fabs(r);
        data.center = c;
        return true;
    } else {
        RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFromCR(): "
                        "Cannot create a circle with radius 0.0.");
        return false;
    }
}



/**
 * Creates this circle from two opposite points.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 */
bool RS_Circle::createFrom2P(const RS_Vector& p1, const RS_Vector& p2) {
        double r=0.5*p1.distanceTo(p2);
    if (r>RS_TOLERANCE) {
        data.radius = r;
        data.center = (p1+p2)*0.5;
        return true;
    } else {
//        RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFrom2P(): "
//                        "Cannot create a circle with radius 0.0.");
        return false;
    }
}



/**
 * Creates this circle from 3 given points which define the circle line.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @param p3 3rd point.
 */
bool RS_Circle::createFrom3P(const RS_Vector& p1, const RS_Vector& p2,
                             const RS_Vector& p3) {
        RS_Vector vra=p2 - p1;
        RS_Vector vrb=p3 - p1;
        double ra2=vra.squared()*0.5;
        double rb2=vrb.squared()*0.5;
        double crossp=vra.x * vrb.y - vra.y * vrb.x;
        if (fabs(crossp)< RS_TOLERANCE2) {
                RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFrom3P(): "
                        "Cannot create a circle with radius 0.0.");
                return false;
        }
        crossp=1./crossp;
        data.center.set((ra2*vrb.y - rb2*vra.y)*crossp,(rb2*vra.x - ra2*vrb.x)*crossp);
        data.radius=data.center.magnitude();
        data.center += p1;
        return true;
}
//*create Circle from 3 points
//Author: Dongxu Li
bool RS_Circle::createFrom3P(const RS_VectorSolutions& sol) {
    if(sol.getNumber() < 2) return false;
    if(sol.getNumber() == 2) return createFrom2P(sol.get(0),sol.get(1));
    if((sol.get(1)-sol.get(2)).squared() < RS_TOLERANCE2)
        return createFrom2P(sol.get(0),sol.get(1));
    RS_Vector vra(sol.get(1) - sol.get(0));
    RS_Vector vrb(sol.get(2) - sol.get(0));
    double ra2=vra.squared()*0.5;
    double rb2=vrb.squared()*0.5;
    double crossp=vra.x * vrb.y - vra.y * vrb.x;
    if (fabs(crossp)< RS_TOLERANCE2) {
        RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFrom3P(): "
                        "Cannot create a circle with radius 0.0.");
        return false;
    }
    crossp=1./crossp;
    data.center.set((ra2*vrb.y - rb2*vra.y)*crossp,(rb2*vra.x - ra2*vrb.x)*crossp);
    data.radius=data.center.magnitude();
    data.center += sol.get(0);
    return true;
}
//
//    if (p1.distanceTo(p2)>RS_TOLERANCE &&
//            p2.distanceTo(p3)>RS_TOLERANCE &&
//            p3.distanceTo(p1)>RS_TOLERANCE) {
//
//        // middle points between 3 points:
//        RS_Vector mp1, mp2;
//        RS_Vector dir1, dir2;
//        double a1, a2;
//
//        // intersection of two middle lines
//        mp1 = (p1 + p2)/2.0;
//        a1 = p1.angleTo(p2) + M_PI/2.0;
//        dir1.setPolar(100.0, a1);
//        mp2 = (p2 + p3)/2.0;
//        a2 = p2.angleTo(p3) + M_PI/2.0;
//        dir2.setPolar(100.0, a2);
//
//        RS_ConstructionLineData d1(mp1, mp1 + dir1);
//        RS_ConstructionLineData d2(mp2, mp2 + dir2);
//        RS_ConstructionLine midLine1(NULL, d1);
//        RS_ConstructionLine midLine2(NULL, d2);
//
//        RS_VectorSolutions sol =
//            RS_Information::getIntersection(&midLine1, &midLine2);
//
//        data.center = sol.get(0);
//        data.radius = data.center.distanceTo(p3);
//
//        if (sol.get(0).valid && data.radius<1.0e14 && data.radius>RS_TOLERANCE) {
//            return true;
//        } else {
//            RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFrom3P(): "
//                            "Cannot create a circle with inf radius.");
//            return false;
//        }
//    } else {
//        RS_DEBUG->print(RS_Debug::D_WARNING, "RS_Circle::createFrom3P(): "
//                        "Cannot create a circle with radius 0.0.");
//        return false;
//    }
//}
/**
  *create circle inscribled in a triangle
  *
  *Author: Dongxu Li
  */
bool RS_Circle::createInscribe(const RS_Vector& coord, const QVector<RS_Line*>& lines){
    if(lines.size()<3) return false;
    QVector<RS_Line*> tri(lines);
    RS_VectorSolutions sol=RS_Information::getIntersectionLineLine(tri[0],tri[1]);
    if(sol.getNumber() == 0 ) {//move parallel to opposite
        std::swap(tri[1],tri[2]);
        sol=RS_Information::getIntersectionLineLine(tri[0],tri[1]);
    }
    if(sol.getNumber() == 0 ) return false;
    RS_Vector vp0(sol.get(0));
    sol=RS_Information::getIntersectionLineLine(tri[2],tri[1]);
    if(sol.getNumber() == 0 ) return false;
    RS_Vector vp1(sol.get(0));
    RS_Vector dvp(vp1-vp0);
    double a(dvp.squared());
    if( a< RS_TOLERANCE2) return false; //three lines share a common intersecting point
    RS_Vector vp(coord - vp0);
    vp -= dvp*(RS_Vector::dotP(dvp,vp)/a); //normal component
    RS_Vector vl0(tri[0]->getEndpoint() - tri[0]->getStartpoint());
    a=dvp.angle();
    double angle0(0.5*(vl0.angle() + a));
    if( RS_Vector::dotP(vp,vl0) <0.) {
        angle0 += 0.5*M_PI;
    }

    RS_Line line0(vp0, vp0+RS_Vector(angle0));//first bisecting line
    vl0=(tri[2]->getEndpoint() - tri[2]->getStartpoint());
    angle0=0.5*(vl0.angle() + a+M_PI);
    if( RS_Vector::dotP(vp,vl0) <0.) {
        angle0 += 0.5*M_PI;
    }
    RS_Line line1(vp1, vp1+RS_Vector(angle0));//second bisection line
    sol=RS_Information::getIntersectionLineLine(&line0,&line1);
    if(sol.getNumber() == 0 ) return false;
    return createFromCR(sol.get(0),tri[1]->getDistanceToPoint(sol.get(0)));
}

QVector<RS_Entity* > RS_Circle::offsetTwoSides(const double& distance) const
{
    QVector<RS_Entity*> ret(0,NULL);
    ret<<new RS_Circle(NULL,RS_CircleData(getCenter(),getRadius()+distance));
    if(getRadius()>distance)
    ret<<new RS_Circle(NULL,RS_CircleData(getCenter(),getRadius()-distance));
    return ret;
}

RS_VectorSolutions RS_Circle::createTan1_2P(const RS_AtomicEntity* circle, const QVector<RS_Vector> points)
{
    RS_VectorSolutions ret;
    if(circle==NULL||points.size()<2) return ret;
    return LC_Quadratic::getIntersection(
                LC_Quadratic(circle,points[0]),
                LC_Quadratic(circle,points[1])
                );
}

/**
  * create a circle of radius r and tangential to two given entities
  */
RS_VectorSolutions RS_Circle::createTan2(const QVector<RS_AtomicEntity*>& circles, const double& r)
{
    if(circles.size()<2) return false;
    auto&& e0=circles[0]->offsetTwoSides(r);
    auto&& e1=circles[1]->offsetTwoSides(r);
    RS_VectorSolutions centers;
    if(e0.size()>0 && e1.size()>=0) {
        for(auto it0=e0.begin();it0!=e0.end();it0++){
            for(auto it1=e1.begin();it1!=e1.end();it1++){
                centers.appendTo(RS_Information::getIntersection(*it0,*it1));
            }
        }
    }
    for(auto it0=e0.begin();it0!=e0.end();it0++){
        delete *it0;
    }
    for(auto it0=e1.begin();it0!=e1.end();it0++){
        delete *it0;
    }
    return centers;

}

QList<RS_Circle> RS_Circle::createTan3(const QVector<RS_AtomicEntity*>& circles)
{
    QList<RS_Circle> ret;
    if(circles.size()!=3) return ret;
     QList<RS_Circle> cs;
     for(unsigned short i=0;i<3;i++){
         cs<<RS_Circle(NULL,RS_CircleData(circles.at(i)->getCenter(),circles.at(i)->getRadius()));
     }
    unsigned short flags=0;
    do{
        ret.append(solveAppolloniusSingle(cs));
        flags++;
        unsigned short j=0;
        for(unsigned short i=1u;i<=4u;i<<=1){
            if(flags & i) {
                cs[j].setRadius( - fabs(cs[j].getRadius()));
            }else{
                cs[j].setRadius( fabs(cs[j].getRadius()));
            }
            j++;
        }

    }while(flags<8u);
//    std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
//    std::cout<<"before testing, ret.size()="<<ret.size()<<std::endl;
    for(int i=0;i<ret.size();){
        if(ret[i].testTan3(circles) == false) {
            ret.erase(ret.begin()+i);
        }else{
            i++;
        }
    }
//        DEBUG_HEADER();
//    std::cout<<"after testing, ret.size()="<<ret.size()<<std::endl;
    return ret;
}

bool RS_Circle::testTan3(const QVector<RS_AtomicEntity*>& circles)
{

    if(circles.size()!=3) return false;

    const auto itEnd=circles.end();
//        std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
//        std::cout<<"to verify Center = ( "<<data.center.x<<" , "<<data.center.y<<" ), r= "<<data.radius<<std::endl;
    for(auto it=circles.begin();it!=itEnd;it++){
        const double r0 = fabs(data.radius);
        const double r1 = fabs((*it)->getRadius());

        const double dist=fabs((data.center - (*it)->getCenter()).magnitude());
//        DEBUG_HEADER();
//        std::cout<<"testing: "<<getCenter()<<" r="<<getRadius()<<". \twith Center = ( "<<(*it)->getCenter().x<<" , "<<(*it)->getCenter().y<<" ), r= "<<(*it)->getRadius()<<std::endl;
//        std::cout<<"r0="<<r0<<"\tr1="<<r1<<"\tdist="<<dist<<"\tdelta0="<<fabs(dist - fabs(r0 - r1)) <<"\tdelta1="<<fabs(dist - fabs(r0 + r1))
//                <<"\t"<<sqrt(DBL_EPSILON)*qMax(r0,r1)<<std::endl;

        if( dist < qMax(r0,r1) )
                return fabs(dist - fabs(r0 - r1)) <= sqrt(DBL_EPSILON)*qMax(r0,r1);
        else
                return fabs(dist - fabs(r0 + r1)) <= sqrt(DBL_EPSILON)*qMax(r0,r1);
    }
    return true;
}

/** solve one of the eight Appollonius Equations
| Cx - Ci|^2=(Rx+Ri)^2
with Cx the center of the common tangent circle, Rx the radius. Ci and Ri are the Center and radius of the i-th existing circle
**/
QList<RS_Circle> RS_Circle::solveAppolloniusSingle(const QList<RS_Circle>& circles)
{
//          std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
//          for(int i=0;i<circles.size();i++){
//std::cout<<"i="<<i<<"\t center="<<circles[i].getCenter()<<"\tr="<<circles[i].getRadius()<<std::endl;
//          }
    QList<RS_Circle> ret;

    QList<RS_Vector> centers;
    QList<double> radii;

    for(size_t i=0;i<3;i++){
        if(circles[i].getCenter().valid==false) return ret;
        centers.push_back(circles[i].getCenter());
        radii.push_back(fabs(circles[i].getRadius()));
    }
/** form the linear equation to solve center in radius **/
    QVector<QVector<double> > mat(2,QVector<double>(3,0.));
    mat[0][0]=centers[2].x - centers[0].x;
    mat[0][1]=centers[2].y - centers[0].y;
    mat[1][0]=centers[2].x - centers[1].x;
    mat[1][1]=centers[2].y - centers[1].y;
    if(fabs(mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0])<RS_TOLERANCE2){
//        DEBUG_HEADER();
//        std::cout<<"The provided circles are in a line, not common tangent circle"<<std::endl;
        size_t i0=0;
        if( centers[0].distanceTo(centers[1]) <= RS_TOLERANCE ||  centers[0].distanceTo(centers[2]) <= RS_TOLERANCE) i0 = 1;
        LC_Quadratic lc0(& (circles[i0]), & (circles[(i0+1)%3]));
        LC_Quadratic lc1(& (circles[i0]), & (circles[(i0+2)%3]));
        auto&& c0 = LC_Quadratic::getIntersection(lc0, lc1);
//        qDebug()<<"c0.size()="<<c0.size();
        for(size_t i=0; i<c0.size(); i++){
            const double dc =  c0[i].distanceTo(centers[i0]);
            ret<<RS_Circle(NULL, RS_CircleData(c0[i], fabs(dc - radii[i0])));
            if( dc > radii[i0]) {
                ret<<RS_Circle(NULL, RS_CircleData(c0[i], dc + radii[i0]));
            }
        }
        return ret;
    }
    // r^0 term
    mat[0][2]=0.5*(centers[2].squared()-centers[0].squared()+radii[0]*radii[0]-radii[2]*radii[2]);
    mat[1][2]=0.5*(centers[2].squared()-centers[1].squared()+radii[1]*radii[1]-radii[2]*radii[2]);
    std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
    for(unsigned short i=0;i<=1;i++){
        std::cout<<"eqs P:"<<i<<" : "<<mat[i][0]<<"*x + "<<mat[i][1]<<"*y = "<<mat[i][2]<<std::endl;
    }
//    QVector<QVector<double> > sm(2,QVector<double>(2,0.));
    QVector<double> sm(2,0.);
    if(RS_Math::linearSolver(mat,sm)==false){
        return ret;
    }

    RS_Vector vp(sm[0],sm[1]);
//      std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
//      std::cout<<"vp="<<vp<<std::endl;

    // r term
    mat[0][2]= radii[0]-radii[2];
    mat[1][2]= radii[1]-radii[2];
//    for(unsigned short i=0;i<=1;i++){
//        std::cout<<"eqs Q:"<<i<<" : "<<mat[i][0]<<"*x + "<<mat[i][1]<<"*y = "<<mat[i][2]<<std::endl;
//    }
    if(RS_Math::linearSolver(mat,sm)==false){
        return ret;
    }
    RS_Vector vq(sm[0],sm[1]);
//      std::cout<<"vq="<<vq<<std::endl;
    //form quadratic equation for r
    RS_Vector dcp=vp-centers[0];
    double a=vq.squared()-1.;
    if(fabs(a)<RS_TOLERANCE*1e-4) {
        return ret;
    }
    std::vector<double> ce(0,0.);
    ce.push_back(2.*(dcp.dotP(vq)-radii[0])/a);
    ce.push_back((dcp.squared()-radii[0]*radii[0])/a);
    std::vector<double>&& vr=RS_Math::quadraticSolver(ce);
    for(size_t i=0; i < vr.size();i++){
        if(vr.at(i)<RS_TOLERANCE) continue;
        ret<<RS_Circle(NULL,RS_CircleData(vp+vq*vr.at(i),vr.at(i)));
    }
//    std::cout<<__FILE__<<" : "<<__FUNCTION__<<" : line "<<__LINE__<<std::endl;
//    std::cout<<"Found "<<ret.size()<<" solutions"<<std::endl;

    return ret;
}

RS_VectorSolutions RS_Circle::getRefPoints() {
    RS_Vector v1(data.radius, 0.0);
    RS_Vector v2(0.0, data.radius);

    RS_VectorSolutions ret(data.center,
                           data.center+v1, data.center+v2,
                           data.center-v1, data.center-v2);
    return ret;
}


/**
 * @return Always an invalid vector.
 */
RS_Vector RS_Circle::getNearestEndpoint(const RS_Vector& /*coord*/, double* dist)const {
    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }
    return RS_Vector(false);
}



RS_Vector RS_Circle::getNearestPointOnEntity(const RS_Vector& coord,
        bool /*onEntity*/, double* dist, RS_Entity** entity)const {

    if (entity!=NULL) {
        *entity = const_cast<RS_Circle*>(this);
    }
    RS_Vector vp(coord - data.center);
    double d(vp.magnitude());
    if( d < RS_TOLERANCE ) return RS_Vector(false);
    vp =data.center+vp*(data.radius/d);
    if(dist!=NULL){
        *dist=coord.distanceTo(vp);
    }
    return vp;
}


/**
  *find the tangential points from a given point, i.e., the tangent lines should pass
  * the given point and tangential points
  *
  *Author: Dongxu Li
  */
RS_VectorSolutions RS_Circle::getTangentPoint(const RS_Vector& point) const {
    RS_VectorSolutions ret;
    double r2(getRadius()*getRadius());
    if(r2<RS_TOLERANCE2) return ret; //circle too small
    RS_Vector vp(point-getCenter());
    double c2(vp.squared());
    if(c2<r2-getRadius()*2.*RS_TOLERANCE) {
        //inside point, no tangential point
        return ret;
    }
    if(c2>r2+getRadius()*2.*RS_TOLERANCE) {
        //external point
        RS_Vector vp1(-vp.y,vp.x);
        vp1*=getRadius()*sqrt(c2-r2)/c2;
        vp *= r2/c2;
        vp += getCenter();
        if(vp1.squared()>RS_TOLERANCE2) {
            ret.push_back(vp+vp1);
            ret.push_back(vp-vp1);
            return ret;
        }
    }
    ret.push_back(point);
    return ret;
}
RS_Vector RS_Circle::getTangentDirection(const RS_Vector& point) const {
    RS_Vector vp(point-getCenter());
//    double c2(vp.squared());
//    if(c2<r2-getRadius()*2.*RS_TOLERANCE) {
//        //inside point, no tangential point
//        return RS_Vector(false);
//    }
    return RS_Vector(-vp.y,vp.x);

}

RS_Vector RS_Circle::getNearestCenter(const RS_Vector& coord,
                                      double* dist) {
    if (dist!=NULL) {
        *dist = coord.distanceTo(data.center);
    }
    return data.center;
}



RS_Vector RS_Circle::getMiddlePoint(void)const
{
    return RS_Vector(false);
}

RS_Vector RS_Circle::getNearestMiddle(const RS_Vector& /*coord*/,
                                      double* dist,
                                      const int /*middlePoints*/
                                      )const {
    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }
    return RS_Vector(false);
}



RS_Vector RS_Circle::getNearestDist(double /*distance*/,
                                    const RS_Vector& /*coord*/,
                                    double* dist) {

    if (dist!=NULL) {
        *dist = RS_MAXDOUBLE;
    }
    return RS_Vector(false);
}

RS_Vector RS_Circle::getNearestDist(double /*distance*/,
                                    bool /*startp*/) {

    return RS_Vector(false);
}


RS_Vector RS_Circle::getNearestOrthTan(const RS_Vector& coord,
                    const RS_Line& normal,
                    bool /*onEntity = false*/)
{
        if ( !coord.valid) {
                return RS_Vector(false);
        }
        RS_Vector vp0(coord-getCenter());
        RS_Vector vp1(normal.getAngle1());
        double d=RS_Vector::dotP(vp0,vp1);
        if(d >= 0. ) {
                return getCenter() + vp1*getRadius();
        }else{
                return getCenter() - vp1*getRadius();
        }
}

double RS_Circle::getDistanceToPoint(const RS_Vector& coord,
                                     RS_Entity** entity,
                                     RS2::ResolveLevel, double) const {
    if (entity!=NULL) {
        *entity = const_cast<RS_Circle*>(this);
    }

    // RVT Jan 6 2010, allow selections to mid point of circle
    double dToCenter=data.center.distanceTo(coord);
    double dToEdge=fabs(dToCenter - data.radius);

    if (dToEdge<dToCenter) {
        return dToEdge;
    } else {
        return dToCenter;
    }
}



void RS_Circle::move(const RS_Vector& offset) {
    data.center.move(offset);
    moveBorders(offset);
//    calculateBorders();
}

/**
  * this function creates offset
  *@coord, position indicates the direction of offset
  *@distance, distance of offset
  * return true, if success, otherwise, false
  *
  *Author: Dongxu Li
  */
bool RS_Circle::offset(const RS_Vector& coord, const double& distance) {
    double r0(coord.distanceTo(getCenter()));
    if(r0 > getRadius()){
        //external
        r0 = getRadius()+ fabs(distance);
    }else{
        r0 = getRadius()- fabs(distance);
        if(r0<RS_TOLERANCE) {
            return false;
        }
    }
    setRadius(r0);
    calculateBorders();
    return true;
}

void RS_Circle::rotate(const RS_Vector& center, const double& angle) {
    data.center.rotate(center, angle);
    calculateBorders();
}

void RS_Circle::rotate(const RS_Vector& center, const RS_Vector& angleVector) {
    data.center.rotate(center, angleVector);
    calculateBorders();
}

void RS_Circle::scale(const RS_Vector& center, const RS_Vector& factor) {
    data.center.scale(center, factor);
    //radius allways is positive
    data.radius *= fabs(factor.x);
    scaleBorders(center,factor);
//    calculateBorders();
}



void RS_Circle::mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) {
    data.center.mirror(axisPoint1, axisPoint2);
    calculateBorders();
}


/** whether the entity's bounding box intersects with visible portion of graphic view
//fix me, need to handle overlay container separately
*/
bool RS_Circle::isVisibleInWindow(RS_GraphicView* view) const
{

    RS_Vector vpMin(view->toGraph(0,view->getHeight()));
    RS_Vector vpMax(view->toGraph(view->getWidth(),0));
    QPolygonF visualBox(QRectF(vpMin.x,vpMin.y,vpMax.x-vpMin.x, vpMax.y-vpMin.y));
    QVector<RS_Vector> vps;
    for(unsigned short i=0;i<4;i++){
        const QPointF& vp(visualBox.at(i));
        vps<<RS_Vector(vp.x(),vp.y());
    }
    for(unsigned short i=0;i<4;i++){
        RS_Line line(NULL,RS_LineData(vps.at(i),vps.at((i+1)%4)));
        RS_Circle c0(NULL, getData());
        if( RS_Information::getIntersection(&c0, &line, true).size()>0) return true;
    }
    if( getCenter().isInWindowOrdered(vpMin,vpMax)==false) return false;
    return (vpMin-getCenter()).squared() > getRadius()*getRadius();
}

/** draw circle as a 2 pi arc */
void RS_Circle::draw(RS_Painter* painter, RS_GraphicView* view, double& patternOffset) {
    RS_Arc arc(getParent(), RS_ArcData(getCenter(),getRadius(),0.,2.*M_PI, false));
    arc.setSelected(isSelected());
    arc.setPen(getPen());
    arc.draw(painter,view,patternOffset);
}


void RS_Circle::moveRef(const RS_Vector& ref, const RS_Vector& offset) {
    if(ref.distanceTo(data.center)<1.0e-4){
        data.center += offset;
        return;
    }
    RS_Vector v1(data.radius, 0.0);
    RS_VectorSolutions sol;
    sol.push_back(data.center + v1);
    sol.push_back(data.center - v1);
    v1.set(0., data.radius);
    sol.push_back(data.center + v1);
    sol.push_back(data.center - v1);
    double dist;
    v1=sol.getClosest(ref,&dist);
    if(dist>1.0e-4) return;
    data.radius = data.center.distanceTo(v1 + offset);
}


/** return the equation of the entity
for quadratic,

return a vector contains:
m0 x^2 + m1 xy + m2 y^2 + m3 x + m4 y + m5 =0

for linear:
m0 x + m1 y + m2 =0
**/
LC_Quadratic RS_Circle::getQuadratic() const
{
    std::vector<double> ce(6,0.);
    ce[0]=1.;
    ce[2]=1.;
    ce[5]=-data.radius*data.radius;
    LC_Quadratic ret(ce);
    ret.move(data.center);
    return ret;
}

/**
 * Dumps the circle's data to stdout.
 */
std::ostream& operator << (std::ostream& os, const RS_Circle& a) {
    os << " Circle: " << a.data << "\n";
    return os;
}

