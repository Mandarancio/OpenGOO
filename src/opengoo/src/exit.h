#ifndef EXIT_H
#define EXIT_H

struct WOGLevelExit;

class Exit
{
    public:
        Exit(WOGLevelExit* exit);
        ~Exit();
        void Update();        
        int Balls() const;
        void Close();

    private:
        struct ExitImpl* pImpl_;
};

#endif // EXIT_H
