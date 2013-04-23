#ifndef EXIT_H
#define EXIT_H

struct WOGLevelExit;

class QPainter;

class Exit
{
    public:
        Exit(WOGLevelExit* exit);
        ~Exit();
        void Update();
        void Painter(QPainter* painter);

    private:
        struct ExitImpl* pImpl_;
};

#endif // EXIT_H
