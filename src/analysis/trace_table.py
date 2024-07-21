import sys
import csv
from vcd.reader import tokenize, TokenKind


def read_vcd(vcd_filename):
    signal_changes = {}
    signal_names = {}
    current_time = 0
    current_scope = None
    timescale = None

    # Open the file in binary mode
    with open(vcd_filename, 'rb') as f:
        for token in tokenize(f):
            if token.kind == TokenKind.TIMESCALE:
                timescale = token.timescale[1].value
            elif token.kind == TokenKind.CHANGE_TIME:
                current_time = token.data
            elif token.kind == TokenKind.SCOPE:
                current_scope = token.data
            elif token.kind == TokenKind.UPSCOPE:
                current_scope = None
            elif token.kind == TokenKind.VAR:
                signal_names[token.data.id_code] = token.data.reference
            elif token.kind in (TokenKind.CHANGE_SCALAR, TokenKind.CHANGE_VECTOR):
                token_id = token.data.id_code
                if token_id not in signal_changes:
                    signal_changes[token_id] = []
                signal_changes[token_id].append((current_time, token.data.value))

    return signal_changes, signal_names, timescale


def tabulate(signal_changes, signal_names, csv_filename, timescale):
    with open(csv_filename, 'w', newline='') as csvfile:
        fieldnames = [f'time in {timescale}'] + [signal_names.get(id_code, id_code) for id_code in signal_changes.keys()
                                                 ]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        timings = sorted(set(time for changes in signal_changes.values() for time, _ in changes))

        for time in timings:
            row = {f'time in {timescale}': time}
            for id_code, changes in signal_changes.items():
                values = [value for t, value in changes if t == time]
                row[signal_names.get(id_code, id_code)] = values[-1] if values else ''
            writer.writerow(row)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python trace_table <vcd_filename>")
        sys.exit(1)

    vcd_file = sys.argv[1]
    signals, signal_names_, timescale_ = read_vcd(vcd_file)
    csv_ = "signal_changes.csv"
    tabulate(signals, signal_names_, csv_, timescale_)
    print(f"CSV file created: {csv_}")
