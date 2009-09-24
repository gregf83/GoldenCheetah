/*
 * Copyright (c) 2009 Eric Murray (ericm@lne.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <assert.h>
#include <QDebug>
#include <qwt_data.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "RideItem.h"
#include "RideFile.h"
#include "PerfPlot.h"
#include "StressCalculator.h"

PerfPlot::PerfPlot() : STScurve(NULL), LTScurve(NULL), SBcurve(NULL)
{


    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    setCanvasBackground(Qt::white);
    setAxisTitle(yLeft, "Stress (BS/Day)");
    setAxisTitle(xBottom, "Time (days)");

    grid = new QwtPlotGrid();
    grid->enableX(false);
    QPen gridPen;
    gridPen.setStyle(Qt::DotLine);
    grid->setPen(gridPen);
    grid->attach(this);
}

void PerfPlot::setStressCalculator(StressCalculator *sc) {
    _sc = sc;
    days = _sc->n();
    startDate = _sc->getStartDate();
    xmin =  0;
    xmax = _sc->n();
}


void PerfPlot::plot() {

    boost::shared_ptr<QSettings> settings = GetApplicationSettings();

    int  num, tics;
    tics = 27;

    setAxisScale(yLeft, _sc->min(), _sc->max());
    num = xmax - xmin;

    /*
       fprintf(stderr,"PerfPlot::plot: xmin = %d xmax = %d num = %d\n",
       xmin, xmax, num);
       */

    // set axis scale
    if (num < 15) {
	tics = 1;
    } else if (num < 71) {
	tics  = 7;
    } else if (num < 141) {
	tics  = 14;
    }
    setAxisScale(xBottom, xmin, xmax,tics);

    setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(startDate));

    if (STScurve) {
	STScurve->detach();
	delete STScurve;
    }
    STScurve = new QwtPlotCurve(settings->value(GC_STS_NAME,tr("Short Term Stress")).toString());
    STScurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    QPen stspen = QPen(Qt::blue);
    stspen.setWidth(2.0);
    STScurve->setPen(stspen);
    STScurve->setData(_sc->getDays()+xmin,_sc->getSTSvalues()+xmin,num);
    STScurve->attach(this);


    if (LTScurve) {
	LTScurve->detach();
	delete LTScurve;
    }
    LTScurve = new QwtPlotCurve(settings->value(GC_LTS_NAME,tr("Long Term Stress")).toString());
    LTScurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    QPen ltspen = QPen(Qt::green);
    ltspen.setWidth(2.0);
    LTScurve->setPen(ltspen);
    LTScurve->setData(_sc->getDays()+xmin,_sc->getLTSvalues()+xmin,num);
    LTScurve->attach(this);


    if (SBcurve) {
	SBcurve->detach();
	delete SBcurve;
    }
    SBcurve = new QwtPlotCurve(settings->value(GC_SB_NAME,tr("Stress Balance")).toString());
    SBcurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    QPen sbpen = QPen(Qt::black);
    sbpen.setWidth(2.0);
    SBcurve->setPen(sbpen);
    SBcurve->setData(_sc->getDays()+xmin,_sc->getSBvalues()+xmin,num);
    SBcurve->attach(this);

    replot();

}


void PerfPlot::resize(int newmin, int newmax)
{
    if (newmin >= 0 && newmin < _sc->n() && newmin < xmax)
	xmin = newmin;
    if (newmax >= 0 && newmax < _sc->n() && newmax > xmin)
	xmax = newmax;

    plot();

}


