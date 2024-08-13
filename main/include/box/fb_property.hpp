#pragma once



#include <functional>
#include <string>
#include <utility>

#include "fb_serializable.hpp"



namespace fb
{
	namespace box
	{
		class PropertyIface : public Serializable
		{
			public:
				virtual bool setFromJson(const std::string& json) = 0;

				void setId(int id);
				
				int getId() const;
			
			private:
				int _id;
		};



		template<class T>
		class PropertyBase : public PropertyIface
		{
			public:
				using ActionSet = std::function<bool(T)>;
				using Tid = int;



				PropertyBase(
					const std::string& name,
					const std::string& description,
					Tid tid,
					const ActionSet& action,
					T value);

				//TODO: create cJson and append key value as str
				std::string toJson() const override;

				//TODO: call converter function and call calback if ok
				bool setFromJson(const std::string& json) override;
			
			private:
				const std::string _name;
				const std::string _description;
				const Tid _tid;
				const ActionSet _action;
				T _value;



				virtual std::pair<bool, T> _strToValue(const std::string& str) const = 0;
				virtual std::string _valueToString(T val) const = 0;
				virtual std::string _minValueToString() const = 0;
				virtual std::string _maxValueToString() const = 0;
				virtual std::string _getValueType() const = 0;
		};



		class PropertyInt : public PropertyBase<int>
		{
			public:
				PropertyInt(
					const std::string& name,
					const std::string& description,
					Tid tid,
					const ActionSet& action,
					int value,
					int minValue,
					int maxValue
				);
			
			private:
				const int _minValue;
				const int _maxValue;


				
				std::pair<bool, int> _strToValue(const std::string& str) const override;
				std::string _valueToString(int val) const override;
				std::string _minValueToString() const override;
				std::string _maxValueToString() const override;
				std::string _getValueType() const override;
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


		class PropertyString : public PropertyBase<std::string>
		{
			public:
				PropertyString(const std::string& name,
					const std::string& description,
					Tid tid,
					const ActionSet& action,
					const std::string& value,
					int minLength,
					int maxLength);
			
			private:
				const int _minLength;
				const int _maxLength;



				std::pair<bool, std::string> _strToValue(const std::string& str) const override;
				std::string _valueToString(std::string val) const override;
				std::string _minValueToString() const override;
				std::string _maxValueToString() const override;
				std::string _getValueType() const override;
		};



		class Property : public Serializable
		{
			public:
				Property(const std::string& name,
					const std::string& description,
					const std::string& valueType,
					int id,
					int tid,
					double minValue,
					double maxValue,
					double value);


				virtual std::string toJson() const override;

				int getId() const;
				
				void setId(int id);
			
			private:
				std::string _name;
				std::string _description;
				std::string _valueType;
				int _id;
				int _tid;
				double _minValue;
				double _maxValue;
				double _value;
		};



		template class PropertyBase<int>;
		template class PropertyBase<std::string>;
	} // namespace box
	
} // namespace fb
