#pragma once
/// <summary>
/// For adding items to combobox
/// </summary>

namespace LCREYE {
    public ref class ComboboxItem
    {
    private:
    protected: System::String^ _text;
    protected: Object^ _value;

    public:
        ComboboxItem(System::String^ text, Object^ value)
        {
            _text = text;
            _value = value;
        }
        property System::String^ Text
        {
            System::String^ get()
            {
                return _text;
            }
            void set(System::String^ value)
            {
                _text = value;
            }
        }

        property Object^ Value
        {
            Object^ get()
            {
                return _value;
            }
            void set(Object^ value)
            {
                _value = value;
            }
        }
        System::String^ ToString() override
        {
            return Text;
        }

    };
};