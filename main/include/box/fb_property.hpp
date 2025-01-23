#pragma once



#include <functional>
#include <utility>

#include "fb_object_base.hpp"



namespace fb
{
	namespace box
	{
		class PropertyIface : public ObjectStaticTid
		{
			public:
				PropertyIface(Tid tid);
				virtual ~PropertyIface() = default;

				virtual bool setFromJson(const std::string& json) = 0;
				
				virtual std::string getValue() const = 0;
		};



		template<class T>
		class PropertyBase : public PropertyIface
		{
			public:
				using ActionSet = std::function<bool(T)>;



				PropertyBase(
					std::string name,
					std::string description,
					Tid tid,
					ActionSet action,
					T value);
				
				virtual void setMinValueStr(const std::string& val) override;
				virtual void setMaxValueStr(const std::string& val) override;

				//TODO: create cJson and append key value as str
				std::string toJson() const override;

				//TODO: call converter function and call calback if ok
				bool setFromJson(const std::string& json) override;

				virtual std::string getValue() const override;

			private:
				const ActionSet _action;
				T _value;



				virtual std::pair<bool, T> _strToValue(const std::string& str) const = 0;
				virtual std::string _valueToString(T val) const = 0;
		};



		//TODO: change to int64_t
		class PropertyInt : public PropertyBase<int>
		{
			public:
				PropertyInt(
					std::string name,
					std::string description,
					Tid tid,
					ActionSet action,
					int value,
					int minValue,
					int maxValue
				);

				PropertyInt(Tid tid, ActionSet action, int value);

			private:
				const int _minValue;
				const int _maxValue;


				
				std::pair<bool, int> _strToValue(const std::string& str) const override;
				std::string _valueToString(int val) const override;
		};


		/*
		class PropertyFloat : public PropertyBase<float>
		{
			public:
				PropertyFloat(const ActionSet& action);
			
			private:
				float _strToValue(const std::string& str) const override;
				std::string _valueToString(float val) const override;
				std::string _minValueToString() const override;
				std::string _maxValueToString() const override;
				std::string _getValueType() const override;
		};
		*/


		//TODO: maybe remove min and max length as arguments let it be defined by tid
		class PropertyString : public PropertyBase<std::string>
		{
			public:
				PropertyString(std::string name,
					std::string description,
					Tid tid,
					ActionSet action,
					std::string value);

				PropertyString(Tid tid, ActionSet action, std::string value);


			private:
				std::pair<bool, std::string> _strToValue(const std::string& str) const override;
				std::string _valueToString(std::string val) const override;
		};



		class PropertyNone : public PropertyString
		{
			public:
				PropertyNone(Tid tid, ActionSet action);
		};



		template class PropertyBase<int>;
		template class PropertyBase<std::string>;
	} // namespace box
	
} // namespace fb
