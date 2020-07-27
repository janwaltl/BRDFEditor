#ifndef BRDFEDITOR_PASS_KEY_HEADER
#define BRDFEDITOR_PASS_KEY_HEADER

namespace brdfEditor {
template <typename Owner>
class PassKey {
private:
	PassKey() {}
	PassKey(const PassKey &) = default;
	PassKey(PassKey &&) = default;
	PassKey &
	operator=(const PassKey &) = default;
	PassKey &
	operator=(PassKey &&) = default;
	friend Owner;
};
} // namespace brdfEditor

#endif