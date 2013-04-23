#include "og_uiscene.h"
#include "og_uibutton.h"
#include "og_data.h"

OGUI* OGUIScene::CreateUI(const QString &name)
{
    QString path = "./res/scenes/";
    QString sceneFile = path + name + ".scene.xml";
    QString resrcFile = path + name + ".resrc.xml";
    QString buttonGroup = name.toLower() + "group";

    WOGScene* sceneData = 0;
    WOGResources* resrcData = 0;
    OGUI* ui = 0;

    if (QFile::exists(sceneFile)) sceneData = OGData::GetScene(sceneFile);
    if (QFile::exists(resrcFile)) resrcData = OGData::GetResources(resrcFile);

    if (sceneFile != 0 && resrcData != 0)
    {
        WOGButtonGroup* g = sceneData->GetButtonGroup(buttonGroup);

        if (g != 0)
        {
            ui = new OGUI;

            Q_FOREACH(WOGButton* btn, g->button)
            {
                ui->AddButton(_CreateButton(btn, resrcData));
            }
        }
    }

    if (sceneData != 0) delete sceneData;
    if (resrcData != 0) delete resrcData;

    return ui;
}

OGUIButton* OGUIScene::_CreateButton(WOGButton* btn, const WOGResources* resrc)
{
    OGUIButton* ui = new OGUIButton(btn);

    QString pathToUpImg = _GetImgPath(btn->up, resrc);
    QString pathToOverImg = _GetImgPath(btn->over, resrc);

    if (!pathToUpImg.isEmpty()) ui->SetUpImage(pathToUpImg);
    if (!pathToOverImg.isEmpty()) ui->SetOverImage(pathToOverImg);

    return ui;
}

QString OGUIScene::_GetImgPath(const QString id, const WOGResources* resrc)
{
    if (!id.isEmpty())
    {
        return resrc->GetResource(WOGResource::IMAGE, id) + ".png";
    }

    return QString();
}
