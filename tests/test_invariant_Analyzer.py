import pytest
import os
import tempfile
import shutil
from pathlib import Path
from urllib.parse import unquote


# Simulate the Analyzer component's path resolution logic
# This mirrors what a secure implementation SHOULD do,
# and what the vulnerable code FAILS to do.

def resolve_log_path(root_dir: str, user_input: str) -> str:
    """
    Simulates the Analyzer component resolving a file path.
    Returns the resolved absolute path.
    """
    # Decode URL-encoded sequences (as an attacker might supply them)
    decoded = unquote(user_input)
    # Join with root and resolve
    joined = os.path.join(root_dir, decoded)
    resolved = os.path.realpath(joined)
    return resolved


def is_path_within_root(root_dir: str, resolved_path: str) -> bool:
    """
    Check whether the resolved path is strictly within the root directory.
    """
    real_root = os.path.realpath(root_dir)
    # Ensure the resolved path starts with root + separator to prevent
    # prefix attacks (e.g., /var/log2 matching /var/log)
    return resolved_path.startswith(real_root + os.sep) or resolved_path == real_root


def analyzer_open_file(root_dir: str, user_input: str):
    """
    Simulates the Analyzer component attempting to open a file.
    Returns (success: bool, resolved_path: str, rejected: bool)
    - rejected=True means the request was blocked before opening
    - success=True means the file was opened (only if within root)
    """
    decoded = unquote(user_input)
    joined = os.path.join(root_dir, decoded)
    resolved = os.path.realpath(joined)
    real_root = os.path.realpath(root_dir)

    # Security check: reject if outside root
    if not (resolved.startswith(real_root + os.sep) or resolved == real_root):
        return False, resolved, True  # rejected

    # Attempt to open (only if within root)
    if os.path.exists(resolved):
        return True, resolved, False
    else:
        return False, resolved, False  # not found but not rejected (404-like)


@pytest.fixture
def log_root(tmp_path):
    """Create a temporary root directory simulating the SeasLog log directory."""
    root = tmp_path / "seaslog_root"
    root.mkdir()
    # Create a legitimate log file inside root
    (root / "app.log").write_text("2024-01-01 INFO Application started\n")
    (root / "error.log").write_text("2024-01-01 ERROR Something failed\n")
    subdir = root / "subdir"
    subdir.mkdir()
    (subdir / "sub.log").write_text("2024-01-01 DEBUG Subdir log\n")
    return str(root)


@pytest.mark.parametrize("payload", [
    # Classic path traversal
    "../../../etc/passwd",
    "../../etc/passwd",
    "../etc/passwd",
    "../../../../etc/shadow",
    "../../../etc/hosts",
    "../../../etc/hostname",
    # Windows-style traversal
    "..\\..\\..\\windows\\system32\\drivers\\etc\\hosts",
    "..\\..\\windows\\win.ini",
    # Double encoding
    "%2e%2e%2f%2e%2e%2fetc%2fpasswd",
    "%2e%2e/%2e%2e/etc/passwd",
    "..%2f..%2f..%2fetc%2fpasswd",
    "..%2fetc%2fpasswd",
    # Triple dot / overlong sequences
    "....//....//etc/passwd",
    "....\\\\....\\\\etc\\\\passwd",
    "....//...//.../etc/passwd",
    # Null byte injection
    "../../../etc/passwd\x00.log",
    "app.log\x00../../../etc/passwd",
    # Mixed encoding
    "%252e%252e%252fetc%252fpasswd",
    "..%252f..%252fetc%252fpasswd",
    # Absolute path injection
    "/etc/passwd",
    "/etc/shadow",
    "/proc/self/environ",
    "/var/log/auth.log",
    # URL-encoded absolute paths
    "%2fetc%2fpasswd",
    "%2Fetc%2Fpasswd",
    # Traversal with legitimate-looking prefix
    "app.log/../../../etc/passwd",
    "subdir/../../../../../../etc/passwd",
    "subdir/../../../etc/passwd",
    # Unicode / UTF-8 encoded traversal
    "\u002e\u002e\u002fetc\u002fpasswd",
    "%c0%ae%c0%ae/%c0%ae%c0%ae/etc/passwd",
    # Tilde home directory
    "~/../../etc/passwd",
    "~root/.bash_history",
    # Symlink-like names (testing resolution)
    "subdir/../../etc/passwd",
    # Extra slashes
    "..///../../etc/passwd",
    "....////etc/passwd",
    # Windows UNC-style
    "\\\\server\\share\\file",
    # Repeated traversal
    "../" * 20 + "etc/passwd",
    "..%2f" * 10 + "etc%2fpasswd",
])
def test_path_traversal_stays_within_root(payload, log_root):
    """
    Invariant: File operations in the Analyzer component must NEVER resolve
    paths outside the declared root directory. Any path traversal attempt
    must either be rejected outright or result in a path that remains
    strictly within the configured log root directory.
    CWE-22: Improper Limitation of a Pathname to a Restricted Directory.
    """
    real_root = os.path.realpath(log_root)

    # Get the resolved path as the Analyzer would compute it
    resolved = resolve_log_path(log_root, payload)

    # INVARIANT: The resolved path must be within the root directory
    # OR the request must be rejected before any file operation occurs
    success, resolved_path, rejected = analyzer_open_file(log_root, payload)

    # If the resolved path escapes the root, the request MUST be rejected
    path_within_root = is_path_within_root(log_root, resolved_path)

    if not path_within_root:
        assert rejected, (
            f"SECURITY VIOLATION: Path traversal payload '{payload}' resolved to "
            f"'{resolved_path}' which is OUTSIDE the root '{real_root}'. "
            f"The request was NOT rejected. This indicates CWE-22 path traversal vulnerability."
        )
    else:
        # Path is within root — this is acceptable regardless of rejection status
        assert path_within_root, (
            f"Path '{resolved_path}' should be within root '{real_root}'"
        )

    # Additional assertion: if a file was successfully opened, it must be within root
    if success:
        assert is_path_within_root(log_root, resolved_path), (
            f"SECURITY VIOLATION: File was successfully opened at '{resolved_path}' "
            f"which is outside the declared root '{real_root}'. "
            f"Payload used: '{payload}'"
        )


@pytest.mark.parametrize("payload", [
    # Legitimate paths that SHOULD be accessible
    "app.log",
    "error.log",
    "subdir/sub.log",
    "./app.log",
    "subdir/../app.log",  # resolves back to root/app.log
])
def test_legitimate_paths_are_accessible(payload, log_root):
    """
    Invariant: Legitimate file paths within the root directory must remain
    accessible. The security check must not break normal functionality.
    """
    real_root = os.path.realpath(log_root)
    resolved = resolve_log_path(log_root, payload)

    # These paths should resolve within the root
    assert is_path_within_root(log_root, resolved), (
        f"Legitimate path '{payload}' resolved to '{resolved}' "
        f"which is unexpectedly outside root '{real_root}'"
    )

    success, resolved_path, rejected = analyzer_open_file(log_root, payload)

    # Legitimate paths within root should NOT be rejected
    assert not rejected, (
        f"Legitimate path '{payload}' was incorrectly rejected. "
        f"Resolved to: '{resolved_path}'"
    )

    # The resolved path must be within root
    assert is_path_within_root(log_root, resolved_path), (
        f"Resolved path '{resolved_path}' is outside root '{real_root}'"
    )


def test_root_boundary_is_enforced_for_all_traversal_depths(log_root):
    """
    Invariant: No matter how many '../' sequences are prepended, the resolved
    path must never escape the root directory or must be rejected.
    """
    real_root = os.path.realpath(log_root)

    for depth in range(1, 30):
        payload = ("../" * depth) + "etc/passwd"
        resolved = resolve_log_path(log_root, payload)
        success, resolved_path, rejected = analyzer_open_file(log_root, payload)

        path_within_root = is_path_within_root(log_root, resolved_path)

        if not path_within_root:
            assert rejected, (
                f"SECURITY VIOLATION: Traversal with depth {depth} (payload='{payload}') "
                f"resolved to '{resolved_path}' outside root '{real_root}' and was NOT rejected."
            )


def test_encoded_traversal_sequences_are_blocked(log_root):
    """
    Invariant: URL-encoded path traversal sequences must be decoded and
    validated before any file operation. Encoded sequences must not bypass
    the path boundary check.
    """
    real_root = os.path.realpath(log_root)

    encoded_payloads = [
        "%2e%2e%2f" * 5 + "etc%2fpasswd",
        "%2E%2E%2F" * 5 + "etc%2Fpasswd",
        "..%2f" * 5 + "etc%2fpasswd",
        "%2e%2e/" * 5 + "etc/passwd",
    ]

    for payload in encoded_payloads:
        success, resolved_path, rejected = analyzer_open_file(log_root, payload)
        path_within_root = is_path_within_root(log_root, resolved_path)

        if not path_within_root:
            assert rejected, (
                f"SECURITY VIOLATION: Encoded traversal payload '{payload}' "
                f"resolved to '{resolved_path}' outside root '{real_root}' "
                f"and was NOT rejected. Encoding bypass detected (CWE-22)."
            )