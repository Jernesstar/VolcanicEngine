#include "Defines.h"

namespace VolcaniCore {

template<typename TInput, class TPredicate>
auto Apply(const List<TInput>& in, TPredicate&& func) {
	bool empty = in.size() == 0;
	if(empty)
		in.emplace();

	using TOutput = std::decay_t<decltype(func(in[0]))>;
	List<TOutput> out;

	if(!empty)
		for(auto& val : in)
			out.push_back(func(val));

	return out;
}

void Replace(std::string& subject, const std::string& search,
			 const std::string& replace)
{
	size_t pos = 0;
	while((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

}