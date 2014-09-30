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

#include <QMdiArea>
#include <QMdiSubWindow>
#include "rs_grid.h"
#include "qc_dialogfactory.h"
#include "qc_applicationwindow.h"
#include "qg_blockwidget.h"

#include "rs_blocklist.h"


/**
 * Provides a new window for editing the active block.
 */
void QC_DialogFactory::requestEditBlockWindow(RS_BlockList* /*blockList*/) {
    RS_DEBUG->print("QC_DialogFactory::requestEditBlockWindow()");

    QC_ApplicationWindow* appWindow = QC_ApplicationWindow::getAppWindow();
    QC_MDIWindow* parent = appWindow->getMDIWindow();
    if (parent!=NULL) {
        //get blocklist from block widget, bug#3497154
        RS_BlockList* blist = appWindow->getBlockWidget() -> getBlockList();
        if (blist !=NULL) {
            RS_Block* blk = blist->getActive();
//            std::cout<<"QC_DialogFactory::requestEditBlockWindow(): size()="<<((blk==NULL)?0:blk->count() )<<std::endl;
            if (blk!=NULL) {
                QC_MDIWindow* w = appWindow->slotFileNew(blk);
                // the parent needs a pointer to the block window and
                //   vice versa
                parent->addChildWindow(w);
                w->getGraphicView()->zoomAuto(false);
                //update grid settings, bug#3443293
                w->getGraphicView()->getGrid()->updatePointArray();
            }
        }
    }
}



/**
 * Closes all windows that are editing the given block.
 */
void QC_DialogFactory::closeEditBlockWindow(RS_Block* block) {
    RS_DEBUG->print("QC_DialogFactory::closeEditBlockWindow");

    QC_ApplicationWindow* appWindow = QC_ApplicationWindow::getAppWindow();
    QMdiArea* mdiAreaCAD = appWindow->getMdiArea();

    if (mdiAreaCAD==NULL) return; //should not happen
    RS_DEBUG->print("QC_DialogFactory::closeEditBlockWindow: workspace found");

    for (int i = 0; i <mdiAreaCAD->subWindowList().size(); ) {
        RS_DEBUG->print("QC_DialogFactory::closeEditBlockWindow: window: %d",
                        i);
        QC_MDIWindow* m = qobject_cast<QC_MDIWindow*>(mdiAreaCAD->subWindowList().at(i)->widget());
        if(m==NULL) {
            mdiAreaCAD->removeSubWindow(mdiAreaCAD->subWindowList().at(i));
            continue;
        }
        RS_DEBUG->print(
                    "QC_DialogFactory::closeEditBlockWindow: got mdi");
        if (m->getDocument()==block) {
            RS_DEBUG->print(
                        "QC_DialogFactory::closeEditBlockWindow: closing mdi");
            m->closeMDI(true, false);

            mdiAreaCAD->removeSubWindow(mdiAreaCAD->subWindowList().at(i));
            continue;
//            m->setAttribute(Qt::WA_DeleteOnClose);//RLZ: to ensure the window is deleted
//            m->close();
        }
        i++;
    }
    //activate a subWindow, bug#3486357
    QMdiSubWindow* subWindow=mdiAreaCAD->currentSubWindow();
    appWindow->slotWindowActivated(subWindow);

    RS_DEBUG->print("QC_DialogFactory::closeEditBlockWindow: OK");
}

