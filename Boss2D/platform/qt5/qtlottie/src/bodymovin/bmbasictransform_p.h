/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the lottie-qt module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BMBASICTRANSFORM_P_H
#define BMBASICTRANSFORM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QPointF>

#include "bmshape_p.h"
#include "bmproperty_p.h"
#include "bmspatialproperty_p.h"

class QJsonObject;

class BMBasicTransform : public BMShape
{
public:
    BMBasicTransform() = default;
    explicit BMBasicTransform(const BMBasicTransform &other);
    BMBasicTransform(const QJsonObject &definition, BMBase *parent = nullptr);

    BMBase *clone() const override;

    void construct(const QJsonObject &definition);

    void updateProperties(int frame) override;
    void render(LottieRenderer &renderer) const override;

    QPointF anchorPoint() const;
    virtual QPointF position() const;
    QPointF scale() const;
    qreal rotation() const;
    qreal opacity() const;

protected:
    BMSpatialProperty m_anchorPoint;
    bool m_splitPosition = false;
    BMSpatialProperty m_position;
    BMProperty<qreal> m_xPos;
    BMProperty<qreal> m_yPos;
    BMProperty2D<QPointF> m_scale;
    BMProperty<qreal> m_rotation;
    BMProperty<qreal> m_opacity;
};

#endif // BMBASICTRANSFORM_P_H