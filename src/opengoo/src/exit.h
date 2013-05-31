#ifndef EXIT_H
#define EXIT_H

#include <memory>

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
        Exit(const Exit&);
        Exit& operator=(const Exit&);

        struct Impl;
        std::unique_ptr<Impl> _pImpl;
};

#endif // EXIT_H
