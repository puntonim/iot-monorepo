from . import remote_log_models as models


class RemoteLogDomain:
    @classmethod
    def get_log(cls, n: int) -> list[models.RemoteLogDbModel]:
        logs: list[models.RemoteLogDbModel] = list(
            models.RemoteLogDbModel.read_from_disk()
        )
        logs.reverse()
        return logs[:n]

    @classmethod
    def create_log(
        cls,
        message: str | None,
        error: str | None,
        app_id: str,
        device_id: str,
        device_ip: str,
    ) -> models.RemoteLogDbModel:
        temp = models.RemoteLogDbModel(
            message=message,
            error=error,
            app_id=app_id,
            device_id=device_id,
            device_ip=device_ip,
        )
        temp.save_to_disk()
        return temp
