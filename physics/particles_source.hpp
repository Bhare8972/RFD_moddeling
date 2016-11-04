

    template<typename ... extra_types>
	void electron_T::save_history(extra_types ... extra_info)
	{
	    history.emplace_back(this, extra_info...);
	}

    template<typename T>
    std::vector<T> electron_T::get_history( T electron_data::*getter() )
    {
        std::vector<T> ret;
        ret.reserve(history.size());
        for(electron_data& datum : history)
        {
            ret.push_back( (datum.*getter)() );
        }
        return ret;
    }
