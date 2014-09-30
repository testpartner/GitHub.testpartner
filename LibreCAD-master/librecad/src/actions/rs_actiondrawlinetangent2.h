/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
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

#ifndef RS_ACTIONDRAWLINETANGENT2_H
#define RS_ACTIONDRAWLINETANGENT2_H

#include <memory>
#include "rs_previewactioninterface.h"


/**
 * This action class can handle user events to draw tangents from circle to
 * circle.
 *
 * @author Andrew Mustun
 */
class RS_ActionDrawLineTangent2 : public RS_PreviewActionInterface {
        Q_OBJECT
private:
    enum Status {
        SetCircle1,     /**< Choose the startpoint. */
        SetCircle2      /**< Choose the circle / arc. */
    };

public:
    RS_ActionDrawLineTangent2(RS_EntityContainer& container,
                              RS_GraphicView& graphicView);
    ~RS_ActionDrawLineTangent2() {}

    static QAction* createGUIAction(RS2::ActionType /*type*/, QObject* /*parent*/);

    virtual RS2::ActionType rtti(){
        return RS2::ActionDrawLineTangent2;
    }
//    virtual void init(int status=0);

    virtual void trigger();
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void updateMouseButtonHints();
    virtual void finish(bool updateTB);

    //virtual void hideOptions();
    //virtual void showOptions();
    virtual void updateMouseCursor();
//    virtual void updateToolBar();

private:
    /** Closest tangent. */
    std::unique_ptr<RS_Line> tangent;
    RS_LineData lineData;
    /** 1st chosen entity */
    RS_Entity* circle1;
    /** 2nd chosen entity */
    RS_Entity* circle2;
    /** Data of new tangent */
    RS_LineData data;
    QVector<RS2::EntityType> circleType;
    bool valid;
};

#endif
