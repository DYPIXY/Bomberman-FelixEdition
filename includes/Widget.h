#include <map>

class Widget 
{
    private:
        static std::map<std::string, std::string> properties;
    
        Widget* Instance() {
            static Widget instance = Widget();
            return &instance;
        }

    public:  
        virtual void Run();
        virtual void RenderDraw();
        virtual void setProperty(const std::string& key, const std::string& value) {
            properties[key] = value;
        }
};