#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"
#include "Common/Utilities.hpp"
#include <cstdint>
#include <memory>

namespace Thumb
{
    // This may look super dumb, but there is no easy way to group these.
    // None that I can figure out, that is.
    enum ThumbOpcodes
    {
        ADC = 0,
        ADD_1,
        ADD_2,
        ADD_3,
        ADD_4,
        ADD_6,
        ADD_5,
        ADD_7,
        AND,
        ASR_1,
        ASR_2,
        B_CONDITIONAL,
        B_UNCONDITIONAL,
        BIC,
        BKPT,
        BL,
        BLX_1,
        BLX_2,
        BX,
        CMN,
        CMP_1,
        CMP_2,
        CMP_3,
        CPS,
        CPY,
        EOR,
        LDMIA,
        LDR_1,
        LDR_2,
        LDR_3,
        LDR_4,
        LDRB_1,
        LDRB_2,
        LDRH_1,
        LDRH_2,
        LDRSB,
        LDRSH,
        LSL_1,
        LSL_2,
        LSR_1,
        LSR_2,
        MOV_1,
        MOV_2,
        MOV_3,
        MUL,
        MVN,
        NEG,
        ORR,
        POP,
        PUSH,
        REV,
        REV16,
        REVSH,
        ROR,
        SBC,
        SETEND,
        STMIA,
        STR_1,
        STR_2,
        STR_3,
        STRB_1,
        STRB_2,
        STRH_1,
        STRH_2,
        SUB_1,
        SUB_2,
        SUB_3,
        SUB_4,
        SWI,
        SXTB,
        SXTH,
        TST,
        UXTB,
        UXTH
    };
    
    std::string ToString(uint32_t opcode);
}

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction(uint16_t instruction) : Instruction(instruction) { }
    InstructionSet GetInstructionSet() const override { return InstructionSet::Thumb; }
    
    virtual uint32_t GetOpcode() const override = 0;
    virtual std::string ToString() const override = 0;
    
    virtual bool IsImmediate() const override { return false; }
};

/*namespace std
{
    template<> class shared_ptr<::ThumbInstruction> : public __shared_ptr<::ThumbInstruction>
    {
    public:
        template<typename _Derived>
        explicit shared_ptr(_Derived* instance) :
            __shared_ptr(static_cast<::ThumbInstruction*>(instance))
        {
            static_assert(!std::is_same<_Derived, ::ThumbInstruction>::value, "Can't construct the pointer with the same class");
        }

        template<typename _Derived, typename _Deleter>
        shared_ptr(_Derived* __p, _Deleter __d) :
            __shared_ptr<::ThumbInstruction>(__p, __d)
        {
            static_assert(!std::is_same<_Derived, ::ThumbInstruction>::value, "Can't construct the pointer with the same class");
        }

        template<typename _Deleter>
        shared_ptr(nullptr_t __p, _Deleter __d) : __shared_ptr<::ThumbInstruction>(__p, __d) { }

        template<typename _Tp1, typename _Deleter, typename _Alloc>
        shared_ptr(_Tp1* __p, _Deleter __d, _Alloc __a)
            : __shared_ptr<::ThumbInstruction>(__p, __d, std::move(__a))
        {
            static_assert(!std::is_same<_Tp1, ::ThumbInstruction>::value, "Can't construct the pointer with the same class");
        }

        template<typename _Deleter, typename _Alloc>
        shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
            : __shared_ptr<::ThumbInstruction>(__p, __d, std::move(__a)) { }

        template <class _Derived>
        operator std::shared_ptr<_Derived> () const
        {
            return To<_Derived>();
        }

        ::ThumbInstruction* get() const { return this->__shared_ptr<::ThumbInstruction>::get(); }

        ::ThumbInstruction operator*() { return this->__shared_ptr<::ThumbInstruction>::operator*(); }
        ::ThumbInstruction* operator->() { return this->__shared_ptr<::ThumbInstruction>::operator->(); }

    private:
        template<class _Derived> std::shared_ptr<_Derived> To() const
        {
            static_assert(std::is_base_of<::ThumbInstruction, _Derived>::value, "Cast type is not inheriting from the base class.");
            return std::static_pointer_cast<_Derived>(*this);
        }
    };
}*/

#endif
