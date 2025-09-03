# Security Policy

## Supported Versions

We actively support the following versions of QtLucide with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

The QtLucide team takes security vulnerabilities seriously. We appreciate your efforts to responsibly disclose your findings.

### How to Report

**Please do not report security vulnerabilities through public GitHub issues.**

Instead, please report security vulnerabilities by emailing:
- **Email**: 64824374+AstroAir@users.noreply.github.com
- **Subject**: [SECURITY] QtLucide Security Vulnerability Report

### What to Include

Please include the following information in your report:

1. **Description**: A clear description of the vulnerability
2. **Impact**: What kind of vulnerability it is and its potential impact
3. **Reproduction**: Step-by-step instructions to reproduce the issue
4. **Environment**: 
   - QtLucide version
   - Qt version
   - Operating system and version
   - Compiler version
5. **Proof of Concept**: If possible, include a minimal code example
6. **Suggested Fix**: If you have ideas for how to fix the vulnerability

### Response Timeline

- **Acknowledgment**: We will acknowledge receipt of your report within 48 hours
- **Initial Assessment**: We will provide an initial assessment within 5 business days
- **Status Updates**: We will keep you informed of our progress
- **Resolution**: We aim to resolve critical vulnerabilities within 30 days

### Disclosure Policy

- We will work with you to understand and resolve the issue
- We will not take legal action against researchers who:
  - Make a good faith effort to avoid privacy violations and disruptions
  - Report vulnerabilities responsibly
  - Give us reasonable time to respond before disclosure
- We will publicly acknowledge your responsible disclosure (unless you prefer to remain anonymous)

## Security Considerations

### Code Execution
QtLucide processes SVG files and user input. While we use Qt's built-in SVG rendering, be aware that:
- SVG files could potentially contain malicious content
- Always validate and sanitize icon names from untrusted sources
- Consider using a whitelist of allowed icon names in security-sensitive applications

### Memory Safety
- QtLucide is written in C++ and uses Qt's memory management
- We follow Qt's best practices for memory safety
- All public APIs are designed to prevent common memory issues

### Input Validation
- Icon names are validated against the known icon set
- Color values are validated through Qt's QColor class
- Scale factors are clamped to safe ranges (0.1 to 2.0)

### Dependencies
QtLucide depends on:
- Qt6 Core, Gui, Widgets, and Svg modules
- Standard C++ library

Keep these dependencies updated to receive security fixes.

## Best Practices for Users

### Secure Usage
1. **Validate Input**: Always validate icon names from untrusted sources
2. **Update Regularly**: Keep QtLucide and Qt updated to the latest versions
3. **Limit Permissions**: Run applications with minimal required permissions
4. **Monitor Dependencies**: Keep track of Qt security updates

### Example Secure Usage
```cpp
// Validate icon names against known icons
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();

QString userIconName = getUserInput(); // From untrusted source
QStringList availableIcons = lucide->availableIcons();

if (availableIcons.contains(userIconName)) {
    QIcon icon = lucide->icon(userIconName);
    // Safe to use
} else {
    // Use default icon or reject request
    QIcon icon = lucide->icon("alert-triangle");
}
```

## Security Updates

Security updates will be:
- Released as patch versions (e.g., 1.0.1, 1.0.2)
- Documented in CHANGELOG.md with security advisory references
- Announced through GitHub Security Advisories
- Tagged with appropriate CVE numbers when applicable

## Contact

For security-related questions or concerns:
- Email: 64824374+AstroAir@users.noreply.github.com
- GitHub: [@AstroAir](https://github.com/AstroAir)

## Acknowledgments

We would like to thank the following individuals for responsibly disclosing security vulnerabilities:

<!-- This section will be updated as we receive and resolve security reports -->

*No security vulnerabilities have been reported yet.*

---

Thank you for helping keep QtLucide and its users safe!
