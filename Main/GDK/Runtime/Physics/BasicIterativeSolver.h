#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include "PhysicsPrivate.h"

namespace GDK
{
    class BasicIterativeSolver : 
        public RuntimeObject<BasicIterativeSolver>,
        public IContactSolver
    {
    public:
        static std::shared_ptr<BasicIterativeSolver> Create();

        // ICollisionSolver
        virtual void Solve(_In_ std::vector<ContactData>& contacts, _In_ float deltaTime) override;

    private:
        BasicIterativeSolver();

        static void SolveVelocity(_In_ ContactData* contact, _In_ float deltaSeconds);
        static void SolvePosition(_In_ ContactData* contact, _In_ float deltaSeconds);
    };
}
